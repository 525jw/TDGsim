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

    // DEVS
    TIME_T t_mnv = -1.0f;
    TIME_T mnvEquation(float speed);
public:
    Maneuver(Engine* engine, Entity* info);

    bool ExtTransFn(const std::string& inPort, const std::any& anyMessage);
    bool OutputFn();
    bool IntTransFn();
    TIME_T TimeAdvanceFn();
};