#include "order_reader.hpp"
#include "SIM/Environment/environment.hpp"

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

    // helper: resolve unit alias like "BLUE-PLT1" -> actual entity id
    auto resolveEntityId = [&](const std::string& unitName) -> int {
        int id = envPtr->QueryEntityIdByName(unitName);
        if (id >= 0) return id;

        static const char* SUFFIXES[] = {
            "-PL", "-LEADER", "-PL-LEADER", "-PLTLEADER", "-PLT-LEADER",
            "-HQ", "-CMD", "-PL01"
        };
        for (const char* s : SUFFIXES) {
            std::string cand = unitName;
            cand += s;
            id = envPtr->QueryEntityIdByName(cand);
            if (id >= 0) return id;
        }

        {
            std::string cand = unitName;
            std::size_t pos = cand.find("-PLT");
            if (pos != std::string::npos) {
                cand.replace(pos, 4, "-PL");
                id = envPtr->QueryEntityIdByName(cand);
                if (id >= 0) return id;
                static const char* SUFFIXES2[] = {
                    "-PL", "-LEADER", "-PL-LEADER", "-PLTLEADER", "-PLT-LEADER",
                    "-HQ", "-CMD", "-PL01"
                };
                for (const char* s : SUFFIXES2) {
                    std::string c2 = cand;
                    c2 += s;
                    id = envPtr->QueryEntityIdByName(c2);
                    if (id >= 0) return id;
                }
            }
        }

        return -1;
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
        if (entityId < 0) continue;
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
        if (hasDestination) {
            order.to = destination;
            order.hasDestination = true;
        }
        companyOrder.orders[entityId].push_back(order);
    }

    return companyOrder;
}

