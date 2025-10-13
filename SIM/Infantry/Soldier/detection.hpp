#pragma once
#include "DEVS/atomic_model.hpp"
#include "common.hpp"
#include "DEVS/logger.hpp"
#include "SIM/Environment/environment.hpp"

class Detection : public AtomicModel{
private:
    // default
    Entity *info;

    // attribute
    std::vector<int> enemyIds;   // tracked enemy ids inside vision
    int vision = 10;

    // DEVS
    TIME_T t_det = 0.0f;
    TIME_T scanInterval = 1.0f;

    TIME_T detEquation() const;
public:
    Detection(Engine* engine, Entity* info);

    bool ExtTransFn(const std::string& inPort, const std::any& anyMessage);
    bool IntTransFn();
    bool OutputFn();
    TIME_T TimeAdvanceFn();

    void RebuildEnemyPosList();
};
