#pragma once
#include "DEVS/atomic_model.hpp"
#include "common.hpp"
#include "DEVS/logger.hpp"
#include "SIM/Environment/environment.hpp"

class Artillery : public AtomicModel{
private:
    Entity info;
    
    // attribute
    Point targetPos = {-1,-1};
    int curAmmo = config::art.ammo;
    
    //DEVS
    TIME_T t_fire = 0.0f;
    
    // RNG
    std::mt19937 rng;
    bool rngInit = false;
    inline void ensureRng() {
        if (!rngInit) {
            rng.seed(env->GetSeed() + this->info.id);
            rngInit = true;
        }
    }
public:
    Artillery(Engine* engine, Entity info);

    bool ExtTransFn(const std::string& inPort, const std::any& anyMessage);
    bool IntTransFn();
    bool OutputFn();
    TIME_T TimeAdvanceFn();
    
    TIME_T fireEquation();
};
