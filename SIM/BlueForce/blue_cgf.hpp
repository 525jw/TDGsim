#pragma once
#include "DEVS/atomic_model.hpp"
#include "common.hpp"
#include "DEVS/logger.hpp"
#include "SIM/Environment/environment.hpp"


class BlueCgf : public AtomicModel{
private:
    int entityId;
    Entity info;

    std::vector<int> enemyIds;
    int vision = 20;

    TIME_T fireFreq = 1.0f;
    float accuracy = 0.3f;
    int targetId = -1;

    // TIME_T t_det = TIME_INF;
    // TIME_T detEquation() const;

    TIME_T t_fire = -1.0f;
    TIME_T fireEquation();

    // RNG
    std::mt19937 rng;
    bool rngInit = false;
    inline void ensureRng() {
        if (!rngInit) {
            rng.seed(env->GetSeed() + entityId);
            rngInit = true;
        }
    }
public:
    BlueCgf(Engine* engine, int entityId, Entity info);

    bool ExtTransFn(const std::string& inPort, const std::any& message);
    bool IntTransFn();
    bool OutputFn();
    TIME_T TimeAdvanceFn();

    void RebuildEnemyPosList();
};