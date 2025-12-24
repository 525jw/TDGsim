#pragma once
#include "DEVS/atomic_model.hpp"
#include "DEVS/logger.hpp"
#include "tdg_sim/common.hpp"
#include "tdg_sim/sim/environment/environment.hpp"

class Artillery : public AtomicModel{
private:
    Entity info;
    
    // attribute
    Point targetPos = {-1,-1};
    int curAmmo = config::art.ammo;
    
    //DEVS
    float t_fire = 0.0f;
    
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
    float TimeAdvanceFn();
    
    float fireEquation();
};
