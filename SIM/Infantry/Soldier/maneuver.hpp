#pragma once
#include "DEVS/atomic_model.hpp"
#include "common.hpp"
#include "DEVS/logger.hpp"
#include "SIM/Environment/environment.hpp"

class Maneuver : public AtomicModel{
private:
    // default
    Entity* info;

    // attribute
    Point nextPos = {-1,-1};
    float curSpeed = -1.0f;
    float curPkill = config::inf.pkill_covered;

    // DEVS
    TIME_T t_mnv = -1.0f;
    TIME_T mnvEquation(float speed);

    // RNG
    std::mt19937 rng;
    bool rngInit = false;
    inline void ensureRng() {
        if (!rngInit) {
            rng.seed(env->GetSeed() + this->info->id);
            rngInit = true;
        }
    }
public:
    Maneuver(Engine* engine, Entity* info);

    bool ExtTransFn(const std::string& inPort, const std::any& anyMessage);
    bool OutputFn();
    bool IntTransFn();
    TIME_T TimeAdvanceFn();
};