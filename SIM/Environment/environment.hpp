#pragma once
#include "DEVS/atomic_model.hpp"
#include "message.hpp"
#include "DEVS/logger.hpp"
#include <utility>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <optional>
#include <algorithm>
#include <cassert>
#include <string>
 
// === Global Environment Access ===
//   - ENV      : pointer access  (ENV->foo(...))
//   - ENV_REF  : reference access (ENV_REF.foo(...))
class Environment;
namespace EnvGlobal {
    inline Environment* ptr = nullptr;

    inline Environment*       Get()  noexcept { return ptr; }
    inline const Environment* CGet() noexcept { return ptr; }
    inline bool Ready() noexcept { return ptr != nullptr; }
}
#define ENV      EnvGlobal::ptr
#define ENV_REF (*EnvGlobal::Get())

class Environment : public AtomicModel{
private:
    unsigned int seed;
    int width,height;
    std::vector<std::vector<TerrainType>> terrainTypeMap; // location -> TerrainType
    std::unordered_map<int, Entity> entities; // entity Id -> entity instance
    std::vector<std::vector<int>> entityIdMap; // location -> entity Id

public:
    Environment(Engine* engine);

    bool ExtTransFn(const std::string& inPort, const std::any& message);
    bool IntTransFn();
    bool OutputFn();
    TIME_T TimeAdvanceFn();

    // === Interfaces ===
    unsigned int GetSeed() const noexcept { return this->seed; }
    int GetWidth()  const noexcept { return this->width; }
    int GetHeight() const noexcept { return this->height; }

    const std::vector<std::vector<TerrainType>>& GetTerrainMap() const noexcept { return terrainTypeMap; }
    const std::vector<std::vector<int>>&         GetEntityMap()  const noexcept { return entityIdMap; }

    bool InBounds(Point p) const noexcept { return (p.x >= 0 && p.y >= 0 && p.y < GetHeight() && p.x < GetWidth()); }

    const TerrainType& GetTerrainAt(Point p) const { 
        assert(InBounds(p));
        return terrainTypeMap[p.y][p.x];
    }
    int GetEntityIdAt(Point p) const {
        assert(InBounds(p));
        return entityIdMap[p.y][p.x];
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

    EnvMoveResponse RequestMoveEntity(int id, Point p);
    EnvKillResponse RequestKillEntity(int id);

    void TempMapGen();

    virtual ~Environment();
};