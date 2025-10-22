#include "hq.hpp"

namespace {
TaskType ParseTaskType(const std::string& value) noexcept {
    if (value == "MOVE") return TaskType::MOVE;
    if (value == "BOMBARD") return TaskType::BOMBARD;
    if (value == "HOLD") return TaskType::HOLD;
    return TaskType::HOLD;
}

Point ParsePoint(const json& pointNode) noexcept {
    if (!pointNode.is_array() || pointNode.size() < 2u) return {0, 0};
    const auto& x = pointNode[0];
    const auto& y = pointNode[1];
    if (!x.is_number() || !y.is_number()) return {0, 0};
    Point p{};
    p.x = static_cast<int>(x.get<double>());
    p.y = static_cast<int>(y.get<double>());
    return p;
}

std::optional<Side> ParseSideFromWho(const json& whoNode) noexcept {
    auto sideIt = whoNode.find("side");
    if (sideIt == whoNode.end() || !sideIt->is_string()) return std::nullopt;

    std::string sideValue = sideIt->get<std::string>();
    std::string normalized;
    normalized.reserve(sideValue.size());
    for (unsigned char ch : sideValue) {
        normalized.push_back(static_cast<char>(std::toupper(ch)));
    }

    if (normalized == "BLUE") return Side::BLUE;
    if (normalized == "RED")  return Side::RED;
    return std::nullopt;
}

const char* ToString(Side side) noexcept {
    switch (side) {
        case Side::BLUE: return "BLUE";
        case Side::RED:  return "RED";
        default:         return "UNKNOWN";
    }
}
} // namespace

CompanyOrd LoadOrderFromFile(const std::string& path,
                             Side sideFilter,
                             const std::unordered_set<int>& allowedRecipients) {
    CompanyOrd companyOrder;
    std::ifstream input(path);
    if (!input) return companyOrder;

    json root;
    try {
        input >> root;
    } catch (...) {
        return companyOrder;
    }

    auto ordersIt = root.find("orders");
    if (ordersIt == root.end() || !ordersIt->is_array()) return companyOrder;

    Environment* envPtr = EnvReady() ? env : nullptr;
    if (!envPtr) return companyOrder;

    const json& orders = *ordersIt;

    // --- helper: resolve unit alias like "BLUE-PLT1" -> actual entity id (e.g., "-PL", "-LEADER", ...)
    auto resolveEntityId = [&](const std::string& unitName) -> int {
        // 1) exact match first
        int id = envPtr->QueryEntityIdByName(unitName);
        if (id >= 0) return id;

        // 2) common suffix tries (edit this list to match your naming convention)
        static const char* SUFFIXES[] = {
            "-PL", "-LEADER", "-PL-LEADER", "-PLTLEADER", "-PLT-LEADER",
            "-HQ", "-CMD", "-PL01"  // add/remove as needed
        };
        for (const char* s : SUFFIXES) {
            std::string cand = unitName;
            cand += s;
            id = envPtr->QueryEntityIdByName(cand);
            if (id >= 0) return id;
        }

        // 3) minor normalization attempts (PLT -> PL) e.g., "BLUE-PLT1" -> "BLUE-PL1"
        //    Only if it literally contains "-PLT"
        {
            std::string cand = unitName;
            std::size_t pos = cand.find("-PLT");
            if (pos != std::string::npos) {
                cand.replace(pos, 4, "-PL"); // replace "-PLT" with "-PL"
                // try cand, cand+"-PL", etc.
                id = envPtr->QueryEntityIdByName(cand);
                if (id >= 0) return id;
                for (const char* s : SUFFIXES) {
                    std::string c2 = cand;
                    c2 += s;
                    id = envPtr->QueryEntityIdByName(c2);
                    if (id >= 0) return id;
                }
            }
        }

        return -1; // not found
    };

    for (const auto& entry : orders) {
        if (!entry.is_object()) continue;

        const auto whoIt = entry.find("who");
        if (whoIt == entry.end() || !whoIt->is_object()) continue;

        const auto orderSide = ParseSideFromWho(*whoIt);
        if (orderSide.has_value() && orderSide.value() != sideFilter) {
            continue;
        }

        const auto unitIt = whoIt->find("unit");
        if (unitIt == whoIt->end() || !unitIt->is_string()) continue;

        const std::string unitName = unitIt->get<std::string>();
        int entityId = resolveEntityId(unitName);
        if (entityId < 0) continue; // unknown unit alias -> skip
        if (!allowedRecipients.empty() && allowedRecipients.count(entityId) == 0) {
            continue;
        }

        TaskType task = TaskType::HOLD;
        auto whatIt = entry.find("what");
        if (whatIt != entry.end() && whatIt->is_object()) {
            auto taskIt = whatIt->find("task");
            if (taskIt != whatIt->end() && taskIt->is_string()) {
                task = ParseTaskType(taskIt->get<std::string>());
            }
        }

        Point destination{0, 0};
        bool hasDestination = false;

        auto whereIt = entry.find("where");
        if (whereIt != entry.end() && whereIt->is_object()) {
            auto pointIt = whereIt->find("point");
            if (pointIt != whereIt->end()) {
                destination = ParsePoint(*pointIt);
                hasDestination = true;
            }
        }
        if (!hasDestination && whatIt != entry.end() && whatIt->is_object()) {
            auto paramsIt = whatIt->find("task_params");
            if (paramsIt != whatIt->end() && paramsIt->is_object()) {
                auto pointIt = paramsIt->find("point");
                if (pointIt != paramsIt->end()) {
                    destination = ParsePoint(*pointIt);
                    hasDestination = true;
                }
            }
        }
        Order order{};
        order.task = task;
        order.to = destination;
        companyOrder.orders.emplace(entityId, order);
    }

    return companyOrder;
}


HQ::HQ(Engine* engine,
       std::vector<int>* membersId,
       Side side,
       std::string ordersFile)
    : AtomicModel(engine),
      hqSide(side),
      ordersFilePath(std::move(ordersFile)) {
    this->engine = engine;

    if (membersId) {
        controlledEntityIds.insert(membersId->begin(), membersId->end());
    }

    this->AddState("WAIT");
    this->AddState("DECIDE");
    this->AddState("Report");

    this->SetCurState("WAIT");

    AddInputPort("Start");
    AddOutputPort("CompanyOrd");
    AddInputPort("InfantryRep");

}

bool HQ::ExtTransFn(const std::string& inPort, const std::any& anyMessage) {
    if (inPort == "Start"){
        // Indirect message — payload contained in json file
        this->SetCurState("DECIDE");
    }else if(inPort == "InfantryRep"){
        this->SetCurState("REPORT");
    }
    return true;
}
bool HQ::OutputFn() {
    if (this->GetCurState() == "DECIDE") {
        CompanyOrd order = LoadOrderFromFile(
            ordersFilePath,
            hqSide,
            controlledEntityIds);

        LogSimulation(this->engine->GetCurrentTime(),this->GetNameWithId(),"LOAD_ORDER");
        if (order.orders.empty()) {
            LogSimulation(this->engine->GetCurrentTime(),this->GetNameWithId(),"LOAD_ORDER"," orders are empty");
        } else {
            for (const auto& [entityId, ord] : order.orders) {
                std::string taskStr;
                switch (ord.task) {
                    case TaskType::MOVE: taskStr = "MOVE"; break;
                    case TaskType::BOMBARD: taskStr = "BOMBARD"; break;
                    case TaskType::HOLD: taskStr = "HOLD"; break;
                    default: taskStr = "UNKNOWN"; break;
                }
                LogSimulation(this->engine->GetCurrentTime(),this->GetNameWithId(),"LOAD_ORDER"," entityId=",entityId," task=",taskStr," to=",ord.to.x,",",ord.to.y);
            }
        }

        std::any anyOrder = order;
        this->AddOutputEvent("CompanyOrd", anyOrder);
    }
    return true;
}
bool HQ::IntTransFn() {
    if(this->GetCurState()=="DECIDE"){
        this->SetCurState("WAIT");
    }
    return true;
}

TIME_T HQ::TimeAdvanceFn() {
    if(this->GetCurState()=="WAIT") return TIME_INF;
    if(this->GetCurState()=="DECIDE") return t_dec;
    if(this->GetCurState()=="REPORT") return t_rep;
    return -1;
}
