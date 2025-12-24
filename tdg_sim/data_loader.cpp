#include "data_loader.hpp"

namespace {
std::string ToLower(std::string s){
    for(char& c : s){
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
    return s;
}
TerrainType ParseTerrainType(const std::string& s){
    std::string k = ToLower(s);
    if (k == "river" || k== "water") return TerrainType::RIVER;
    return TerrainType::PLAIN;
}
SideType ParseSide(const std::string& s){
    std::string k = ToLower(s);
    if (k == "red")  return SideType::RED;
    return SideType::BLUE;
}
ForceType ParseForceType(const std::string& s){
    std::string k = ToLower(s);
    if (k == "tank") return ForceType::TANK;
    if (k == "artillery") return ForceType::ARTILLERY;
    return ForceType::RIFLE;
}
TaskType ParseTaskType(const std::string& s){
    std::string k = ToLower(s);
    if (k == "move") return TaskType::MOVE;
    if (k == "bombard") return TaskType::BOMBARD;
    return TaskType::HOLD;
}
Point ParsePoint(const json& node) noexcept {
    if (!node.is_array() || node.size() < 2) return {0, 0};
    if (!node[0].is_number() || !node[1].is_number()) return {0, 0};
    return {
        static_cast<int>(node[0].get<double>()),
        static_cast<int>(node[1].get<double>())
    };
}
}

namespace data_loader {
// scenario.json -> struct Scenario
bool LoadScenarioFromFile(std::string_view path, Scenario& out){
    std::ifstream ifs{std::string(path)};
    if (!ifs.is_open()){
        return false;
    }
    json j;
    ifs >> j;

    out = Scenario{};

    // size
    if (j.contains("size") && j["size"].is_object()) {
        const auto& size = j["size"];
        out.width  = size.value("w", 0);
        out.height = size.value("h", 0);
    }

    // seed, default 0 -> generator가 생성
    out.seed = j.value("seed", 0u);
    
    // goal areas
    out.goalRects.clear();
    if (j.contains("goal") && j["goal"].is_array()) {
        for (const auto& area : j["goal"]) {
            const float score = area.value("score", 0.0f);
            Rect rect{
                area.value("x1", 0),
                area.value("y1", 0),
                area.value("x2", area.value("x1", 0)),
                area.value("y2", area.value("y1", 0))
            };
            if (rect.x1 > rect.x2) std::swap(rect.x1, rect.x2);
            if (rect.y1 > rect.y2) std::swap(rect.y1, rect.y2);
            rect.x1 = std::clamp(rect.x1, 0, out.width  - 1);
            rect.x2 = std::clamp(rect.x2, 0, out.width  - 1);
            rect.y1 = std::clamp(rect.y1, 0, out.height - 1);
            rect.y2 = std::clamp(rect.y2, 0, out.height - 1);
            out.goalRects.emplace_back(score, rect);
        }
    }

    // terrain areas
    out.terrainRects.clear();
    if (j.contains("terrain") && j["terrain"].is_array()) {
        for (const auto& area : j["terrain"]) {
            TerrainType terrain = TerrainType::PLAIN;
            if (area.contains("kind") && area["kind"].is_string()) {
                terrain = ParseTerrainType(area["kind"].get<std::string>());
            }
            Rect rect{
                area.value("x1", 0),
                area.value("y1", 0),
                area.value("x2", area.value("x1", 0)),
                area.value("y2", area.value("y1", 0))
            };
            if (rect.x1 > rect.x2) std::swap(rect.x1, rect.x2);
            if (rect.y1 > rect.y2) std::swap(rect.y1, rect.y2);
            rect.x1 = std::clamp(rect.x1, 0, out.width  - 1);
            rect.x2 = std::clamp(rect.x2, 0, out.width  - 1);
            rect.y1 = std::clamp(rect.y1, 0, out.height - 1);
            rect.y2 = std::clamp(rect.y2, 0, out.height - 1);
            out.terrainRects.emplace_back(terrain, rect);
        }
    }

    // entity
    // single entity라면 anchor에 바로 배치, multiple entity라면 count 만큼 anchor가 좌상단에 오게 배치, id는 끝에 -0n 붙이기
    out.entities.clear();
    if (j.contains("entity") && j["entity"].is_array()) {
        for (const auto& entity : j["entity"]) {
            const std::string name = entity.value("id", std::string{});
            if (name.empty()) continue;

            const int count = std::max(1, entity.value("count", 1));
            Point anchorPos{entity.value("x", 0), entity.value("y", 0)};
            if (entity.contains("anchor") && entity["anchor"].is_object()) {
                const auto& anchor = entity["anchor"];
                anchorPos.x = anchor.value("x", anchorPos.x);
                anchorPos.y = anchor.value("y", anchorPos.y);
            }

            const auto side = ParseSide(entity.value("side", std::string{"BLUE"}));
            const auto force = ParseForceType(entity.value("type", std::string{"rifle"}));
            auto clampPos = [&](Point p) {
                p.x = std::clamp(p.x, 0, out.width  - 1);
                p.y = std::clamp(p.y, 0, out.height - 1);
                return p;
            };
            auto makeSuffix = [](int index) {
                std::string suffix = "-SOL";
                if (index < 10) suffix += "0";
                suffix += std::to_string(index);
                return suffix;
            };

            if (count <= 1) {
                Entity e{};
                e.name = name;
                e.side = side;
                e.forceType = force;
                e.position = clampPos(anchorPos);
                out.entities.push_back(std::move(e));
                continue;
            }

            const int cols = std::max(1, static_cast<int>(std::ceil(std::sqrt(static_cast<double>(count)))));
            for (int idx = 0; idx < count; ++idx) {
                Entity e{};
                e.name = name + makeSuffix(idx + 1);
                e.side = side;
                e.forceType = force;
                const int row = idx / cols;
                const int col = idx % cols;
                Point pos{anchorPos.x + col, anchorPos.y + row};
                e.position = clampPos(pos);
                out.entities.push_back(std::move(e));
            }
        }
    }
    return true;
}
// bml.json -> struct CompanyOrd
bool LoadOrderFromFile(std::string_view bmlPath, SideType sideFilter, CompanyOrd& out){
    out = CompanyOrd{};

    std::ifstream input{std::string(bmlPath)};
    if (!input) return false;

    json root;
    try {
        input >> root;
    } catch (...) {
        return false;
    }

    const auto ordersIt = root.find("orders");
    if (ordersIt == root.end() || !ordersIt->is_array()) return false;

    Environment* envPtr = EnvReady() ? env : nullptr;
    if (!envPtr) return false;

    for (const auto& entry : *ordersIt) {
        if (!entry.is_object()) continue;

        auto unitIt = entry.find("entity");
        if (unitIt == entry.end() || !unitIt->is_string()) continue;

        auto taskIt = entry.find("task");
        if (taskIt == entry.end() || !taskIt->is_string()) continue;

        const std::string unitName = unitIt->get<std::string>();
        std::string resolvedName = unitName;
        bool usedLeaderFallback = false;
        auto queryEntityId = [&](const std::string& name) {
            return envPtr->QueryEntityIdByName(name);
        };

        int entityId = queryEntityId(unitName);
        if (entityId < 0) {
            const std::string lowerName = ToLower(unitName);
            const bool hasLeaderSuffix =
                lowerName.rfind("-leader") != std::string::npos;
            if (!hasLeaderSuffix) {
                resolvedName = unitName + "-LEADER";
                entityId = queryEntityId(resolvedName);
                if (entityId < 0) {
                    resolvedName = unitName + "-leader";
                    entityId = queryEntityId(resolvedName);
                }
                usedLeaderFallback = entityId >= 0;
            }
        }
        if (entityId < 0) continue;

        const Entity* entity = envPtr->QueryEntityById(entityId);
        if (!entity) {
            if (!usedLeaderFallback) continue;
            const std::string nameLower = ToLower(resolvedName);
            const std::string sidePrefix =
                (sideFilter == SideType::BLUE) ? "blue-" : "red-";
            if (nameLower.rfind(sidePrefix, 0) != 0) continue;
        } else if (entity->side != sideFilter) {
            continue;
        }

        Order order{};
        order.task = ParseTaskType(taskIt->get<std::string>());

        const json* pointNode = nullptr;
        if (auto point = entry.find("point"); point != entry.end()) {
            pointNode = &*point;
        } else if (auto to = entry.find("to"); to != entry.end()) {
            pointNode = &*to;
        }
        if (pointNode && pointNode->is_array()) {
            order.to = ParsePoint(*pointNode);
            order.hasDestination = true;
        }

        out.orders[entityId].push_back(order);
    }
    return true;
}

} // namespace data_loader
