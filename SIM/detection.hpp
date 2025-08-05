#pragma once
#include "DEVS/atomic_model.hpp"
#include "message.hpp"
#include "DEVS/logger.hpp"
#include <utility>
#include <iostream>
#include <vector>
#include <any>

class Detection : public AtomicModel{
private:
    int objectID;

    std::pair<int,int> curPos={-1, -1}; // (x,y)
    std::unordered_map<int, std::pair<int,int>> enemyPos;   // key: Enemy objectID, value: enemy_pos (x,y)
    int vision = 10;

    TIME_T t_det = TIME_INF;
    void   RebuildEnemyDistance(const EnvInfo& info);
    TIME_T detEquation() const;
public:
    Detection(Engine* engine, int objectID);

    bool ExtTransFn(const std::string& inPort, const std::any& anyMessage);
    bool IntTransFn();
    bool OutputFn();
    TIME_T TimeAdvanceFn();
};