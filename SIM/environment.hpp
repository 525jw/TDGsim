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

struct Entity { int x, y; ForceType forceType; std::string name; };

class Environment : public AtomicModel{
private:
    unsigned int seed;
    std::vector<std::vector<TerrainType>> terrainMap; // location -> TerrainType
    std::vector<std::vector<int>> entityMap; // location -> entity ID
    std::unordered_map<int, Entity> entities; // entity ID -> location, ForceType, name

public:
    Environment(Engine* engine);

    bool ExtTransFn(const std::string& inPort, const std::any& message);
    bool IntTransFn();
    bool OutputFn();
    TIME_T TimeAdvanceFn();

    unsigned int GetSeed() const noexcept { return seed; }
    const std::vector<std::vector<TerrainType>>& GetTerrainMap() const noexcept { return terrainMap; }
    const std::unordered_map<int, Entity>&       GetEntities()   const noexcept { return entities;  }
    const std::vector<std::vector<int>>&         GetEntityMap()  const noexcept { return entityMap; }
    int GetWidth()  const noexcept { return terrainMap.empty() ? 0 : static_cast<int>(terrainMap.front().size()); }
    int GetHeight() const noexcept { return static_cast<int>(terrainMap.size()); }

    bool InBounds(int x, int y) const noexcept { return (x >= 0 && y >= 0 && y < GetHeight() && x < GetWidth()); }

    const TerrainType& GetTerrainAt(int x, int y) const { 
        assert(InBounds(x,y));
        return terrainMap[y][x];
    }

    int GetEntityIdAt(int x, int y) const {
        assert(InBounds(x,y));
        return entityMap[y][x];
    }

    const Entity* GetEntity(int id) const {
        auto it = entities.find(id);
        return (it == entities.end()) ? nullptr : &it->second;
    }

    bool HasEntity(int id) const { return entities.find(id) != entities.end(); }


    void InitMap(int w, int h);
    void PaintRect(int x0,int y0,int x1,int y1, TerrainType t);
    void PaintEllipse(int cx,int cy,int rx,int ry, TerrainType t);
    void PaintRoadVertical(int x, int y0, int y1, int width=8);
    void PaintRiverBand(int yCenter, int thickness=60);
    void PaintBridge(int x, int y0, int y1, int width=8);
    bool SpawnEntity(int id, int x, int y, ForceType ft);
    void InitScenario_BlueRiver();
};