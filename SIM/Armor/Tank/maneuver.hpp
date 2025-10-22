#pragma once
#include "DEVS/atomic_model.hpp"
#include "common.hpp"
#include "DEVS/logger.hpp"
#include "SIM/Environment/environment.hpp"

// Tank-specific maneuver: identical to infantry except
// - movement speed can differ
// - incoming fire leads to probabilistic kill instead of deterministic
class TankManeuver : public AtomicModel{
private:
    // default
    Entity* info;

    // attribute
    Point nextPos = {-1,-1};
    float curSpeed = -1.0f;
    float moveSpeed = 3.0f;    // tank movement speed (cells per tick-equivalent)
    float pKillFromTank = 0.7f;     // probability of being killed by tank direct fire
    float pKillFromArtillery = 0.4f;// probability of being killed by artillery hitting cell
    float pKillFromRifle = 0.0f;    // rifles have negligible effect on tanks by default
    TIME_T lookAroundPeriod = 2.0f;

    // DEVS
    TIME_T t_mnv = -1.0f;
    TIME_T mnvEquation(float speed);

    // RNG
    std::mt19937 rng;
    bool rngInit = false;
    inline void ensureRng() {
        if (!rngInit) {
            rng.seed(env->GetSeed() + this->info->id * 7919u);
            rngInit = true;
        }
    }
public:
    TankManeuver(Engine* engine, Entity* info);

    bool ExtTransFn(const std::string& inPort, const std::any& anyMessage);
    bool OutputFn();
    bool IntTransFn();
    TIME_T TimeAdvanceFn();
};

