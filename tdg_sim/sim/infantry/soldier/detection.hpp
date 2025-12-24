#pragma once
#include "DEVS/atomic_model.hpp"
#include "tdg_sim/common.hpp"
#include "DEVS/logger.hpp"
#include "tdg_sim/sim/environment/environment.hpp"

class Detection : public AtomicModel{
private:
    // default
    Entity *info;

    // attribute
    std::vector<int> enemyIds;   // tracked enemy ids inside vision

    // DEVS
    float t_det = 0.0f;
    float detEquation() const;
public:
    Detection(Engine* engine, Entity* info);

    bool ExtTransFn(const std::string& inPort, const std::any& anyMessage);
    bool IntTransFn();
    bool OutputFn();
    float TimeAdvanceFn();

    void RebuildEnemyPosList();
};
