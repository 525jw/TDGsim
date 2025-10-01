#include "blue_hq.hpp"


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

CompanyOrd LoadOrderFromFile(const std::string& path) {
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

    for (const auto& entry : orders) {
        if (!entry.is_object()) continue;

        auto whoIt = entry.find("who");
        if (whoIt == entry.end() || !whoIt->is_object()) continue;
        auto unitIt = whoIt->find("unit");
        if (unitIt == whoIt->end() || !unitIt->is_string()) continue;

        const std::string unitName = unitIt->get<std::string>();
        int entityId = envPtr->QueryEntityIdByName(unitName);
        if (entityId < 0) continue;

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

BlueHQ::BlueHQ(Engine* engine, std::vector<int> *membersId)
    : AtomicModel(engine)
{
    this->engine=engine;

    this->AddState("WAIT");
    this->AddState("DECIDE");

    this->SetCurState("WAIT");

    AddInputPort("Start");
    AddOutputPort("CompanyOrd");
    AddInputPort("InfantryRep");
}

bool BlueHQ::ExtTransFn(const std::string& inPort, const std::any& anyMessage) {
    if (inPort == "Start"){
        this->SetCurState("DECIDE");
    }else if(inPort == "InfantryRep"){
        this->SetCurState("DECIDE");
    }
    return true;
}
bool BlueHQ::OutputFn() {
    if (this->GetCurState()=="DECIDE"){
        CompanyOrd order = LoadOrderFromFile("out.json");
        std::any anyOrder = order;
        this->AddOutputEvent("CompanyOrd",anyOrder);
    }
    return true;
}
bool BlueHQ::IntTransFn() {
    
    return true;
}

TIME_T BlueHQ::TimeAdvanceFn() {
    if(this->GetCurState()=="WAIT") return TIME_INF;
    if(this->GetCurState()=="DECIDE") return t_dec;
    return -1;
}
