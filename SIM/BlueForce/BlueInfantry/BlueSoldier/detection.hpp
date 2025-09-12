#pragma once
#include "DEVS/atomic_model.hpp"
#include "message.hpp"
#include "DEVS/logger.hpp"
#include "SIM/Environment/environment.hpp"
#include <utility>
#include <iostream>
#include <vector>
#include <any>

class Detection : public AtomicModel{
private:
    int entityId;
    Entity *info;

    std::vector<int> enemyIds;   // key: Enemy entityId, value: enemy pos (x,y)
    int vision = 10;

    TIME_T t_det = TIME_INF;
    TIME_T detEquation() const;
public:
    Detection(Engine* engine, int entityId, Entity* info);

    bool ExtTransFn(const std::string& inPort, const std::any& anyMessage);
    bool IntTransFn();
    bool OutputFn();
    TIME_T TimeAdvanceFn();

    void RebuildEnemyPosList();
};