#pragma once
#include "DEVS/atomic_model.hpp"
#include "DEVS/logger.hpp"
#include "tdg_sim/common.hpp"

class Environment;
extern Environment* env;
inline bool EnvReady() noexcept { return env != nullptr; }

class Environment : public AtomicModel{
private:
    unsigned int seed;
    int width,height;
    std::vector<std::vector<TerrainType>> terrainMap; // location -> TerrainType
    
    std::unordered_map<int, Entity> initEntities; // entity Id -> entity instance (at initialization)
    std::unordered_map<int, Entity> entities; // entity Id -> entity instance
    std::unordered_map<std::string, int> nameToId; // name -> id
    int nextId = 1;
public:
    Environment(Engine* engine);

    bool ExtTransFn(const std::string& inPort, const std::any& message);
    bool IntTransFn();
    bool OutputFn();
    float TimeAdvanceFn();

    // === Interfaces ===
    unsigned int GetSeed() const noexcept { return this->seed; }
    int GetWidth()  const noexcept { return this->width; }
    int GetHeight() const noexcept { return this->height; }

    const std::vector<std::vector<TerrainType>>& GetTerrainMap() const noexcept { return terrainMap; }

    bool InBounds(Point p) const noexcept { return (p.x >= 0 && p.y >= 0 && p.y < GetHeight() && p.x < GetWidth()); }

    const TerrainType& GetTerrainAt(Point p) const {
        assert(InBounds(p));
        return terrainMap[p.y][p.x];
    }
    // 탐색 실패 : return nullptr, 성공 : return Entity*
    const Entity* QueryEntityById(int id) const noexcept {
        auto it = entities.find(id);
        return (it == entities.end()) ? nullptr : &it->second;
    }
    // 탐색 실패 : return {-1,-1}, 성공 : return Point
    Point QueryEntityPosById(int id) const noexcept {
        auto it = entities.find(id);
        if (it == entities.end()) return {-1,-1};
        return it->second.position;
    }
    // O(# of entities), no dict for position -> ids
    std::vector<int> QueryEntityIdsAt(Point p) const {
        std::vector<int> result;
        if (!InBounds(p)) return result;
        for (const auto& kv : entities) {
            if (kv.second.position == p) {
                result.push_back(kv.first);
            }
        }
        return result;
    }
    int QueryEntityIdByName(const std::string& name) const {
        auto it = nameToId.find(name);
        return (it != nameToId.end()) ? it->second : -1;
    }

    int RegisterEntityIdByName(const std::string& name);
    EnvMoveResponse RequestMoveEntity(int id, Point p);
    EnvKillResponse RequestKillEntity(int id);

    // - parameter = force type filter(default : no filter) / returns = (blue count, red count) 
    std::pair<int,int> QueryInitialEntityCounts(
        ForceType fr = ForceType::DEFAULT
    ) const noexcept { 
        int bluecnt = 0; int redcnt = 0; 
        for (const auto& e : initEntities) { 
            if (fr != ForceType::DEFAULT && e.second.forceType != fr) continue; // force type filter
            if (e.second.side == SideType::BLUE) bluecnt++; 
            else if (e.second.side == SideType::RED) redcnt++; 
        } 
        return {bluecnt, redcnt}; 
    }
    // - parameter = force type filter(default : no filter), area filter (default : map range) / returns = (blue count, red count)
    std::pair<int,int> QueryEntityCounts(
        ForceType ft = ForceType::DEFAULT,
        std::optional<Rect> rect = std::nullopt
    ) const noexcept {
        int bluecnt = 0, redcnt = 0;
        if (!rect.has_value()) rect = {0, 0, this->width - 1, this->height - 1}; // default : map range
        for (const auto& e : entities) {
            if (ft != ForceType::DEFAULT && e.second.forceType != ft) continue; // force type filter
            if (rect && !rect->Contains(e.second.position)) continue; // area filter
            if (e.second.side == SideType::BLUE) bluecnt++;
            else if (e.second.side == SideType::RED) redcnt++;
        }
        return {bluecnt, redcnt};
    }

    virtual ~Environment();
};
