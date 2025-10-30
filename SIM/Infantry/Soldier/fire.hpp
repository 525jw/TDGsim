#pragma once
#include "DEVS/atomic_model.hpp"
#include "common.hpp"
#include "DEVS/logger.hpp"
#include "SIM/Environment/environment.hpp"

class Fire : public AtomicModel{
private:
    // default
    Entity* info;

    // attribute
    int targetId = -1;

    // DEVS
    TIME_T t_fire = -0.5f; // TODO : 이런거 에러뜨는 음수도 DEVS/model.cpp 에 넣어야겠다
    TIME_T fireEquation();

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
    Fire(Engine* engine, Entity* info);

    bool ExtTransFn(const std::string& inPort, const std::any& message);
    bool IntTransFn();
    bool OutputFn();
    TIME_T TimeAdvanceFn();
};