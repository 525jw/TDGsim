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
 
class Environment;
namespace EnvGlobal {
    inline const Environment* ptr = nullptr;

    inline const Environment* Get() noexcept { return ptr; }
    inline bool Ready() noexcept { return ptr != nullptr; }
}

#define ENV      EnvGlobal::ptr          // 포인터: ENV->GetTerrainAt(...)
#define ENV_REF (*EnvGlobal::Get())      // 레퍼런스: ENV_REF.GetTerrainAt(...)

struct Entity { Team team; Point location; ForceType forceType; std::string name; };


class Environment : public AtomicModel{
private:
    unsigned int seed;
    std::vector<std::vector<TerrainType>> terrainTypeMap; // location -> TerrainType
    std::vector<std::vector<int>> entityIdMap; // location -> entity Id
    std::unordered_map<int, Entity> entities; // entity Id -> team, location, ForceType, name
    int nextEntityId = 1;

public:
    Environment(Engine* engine);

    bool ExtTransFn(const std::string& inPort, const std::any& message);
    bool IntTransFn();
    bool OutputFn();
    TIME_T TimeAdvanceFn();

    // interface
    unsigned int GetSeed() const noexcept { return seed; }
    const std::vector<std::vector<TerrainType>>& GetTerrainMap() const noexcept { return terrainTypeMap; }
    const std::vector<std::vector<int>>&         GetEntityMap()  const noexcept { return entityIdMap; }
    int GetWidth()  const noexcept { return terrainTypeMap.empty() ? 0 : static_cast<int>(terrainTypeMap.front().size()); }
    int GetHeight() const noexcept { return static_cast<int>(terrainTypeMap.size()); }

    bool InBounds(Point p) const noexcept { return (p.x >= 0 && p.y >= 0 && p.y < GetHeight() && p.x < GetWidth()); }

    const TerrainType& GetTerrainAt(Point p) const { 
        assert(InBounds(p));
        return terrainTypeMap[p.y][p.x];
    }
    int GetEntityIdAt(Point p) const {
        assert(InBounds(p));
        return entityIdMap[p.y][p.x];
    }
    // 탐색 실패 시 return nullptr, 성공 시 Point*
    const Entity* SearchEntityById(int id) const noexcept { 
        auto it = entities.find(id);
        return (it == entities.end()) ? nullptr : &it->second;
    }
    // 탐색 실패 시 return false, 성공 시 out에 결과 넣음
    bool SearchEntityById(int id, const Entity*& out) const noexcept { 
        out = SearchEntityById(id);
        return (out != nullptr);
    }

    // --- 엔티티 스폰/등록 ---
    // 명시 ID로 스폰
    bool SpawnEntity(int id, Point pos, Team team, ForceType ft, std::string_view name);
    // ID 자동 발급 + 등록
    int RegisterEntity(Point pos, Team team, ForceType ft, std::string_view name);
};