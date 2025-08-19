#pragma once
#include "DEVS/atomic_model.hpp"
#include "message.hpp"
#include "DEVS/logger.hpp"
#include "SIM/environment.hpp"
#include <utility>
#include <iostream>
#include <random>

class Fire : public AtomicModel{
private:
    int entityId;

    // TIME_T fireFreq = 0.01f;
    // int ammo = 30;
    TIME_T reloadTime = 1.0f;
    float accuracy = 0.7f;
    int targetId = -1;

    // RNG
    std::mt19937 rng;
    bool rngInit = false;

    TIME_T t_fire = -1.0f;
    TIME_T fireEquation();
public:
    Fire(Engine* engine, int entityId);

    bool ExtTransFn(const std::string& inPort, const std::any& message);
    bool IntTransFn();
    bool OutputFn();
    TIME_T TimeAdvanceFn();
};