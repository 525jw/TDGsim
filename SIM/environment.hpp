#pragma once
#include "DEVS/atomic_model.hpp"
#include "message.hpp"
#include "DEVS/logger.hpp"
#include <utility>
#include <iostream>
#include <vector>

class Environment : public AtomicModel{
private:
    std::unordered_map<int, std::pair<int, int>> blueTeam;
    std::unordered_map<int, std::pair<int, int>> redTeam;

    // std::vector<std::vector<TerrainType>> TerrainMap;
    // std::vector<std::pair<int,int>> obstacles;
public:
    Environment(Engine* engine);

    bool ExtTransFn(const std::string& inPort, const std::any& message);
    bool IntTransFn();
    bool OutputFn();
    TIME_T TimeAdvanceFn();
};