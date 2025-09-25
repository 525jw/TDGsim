#pragma once
#include "DEVS/atomic_model.hpp"
#include "message.hpp"
#include "DEVS/logger.hpp"
#include "SIM/Environment/environment.hpp"
#include <utility>
#include <iostream>

class Maneuver : public AtomicModel{
private:
    int entityId;
    Entity* info; // contains curPos
    Point nextPos = {-1,-1};
    float curSpeed = -1.0f;

    TIME_T t_mnv = -1.0f;
    TIME_T mnvEquation(float speed);
public:
    Maneuver(Engine* engine, int entityId, Entity* info);

    bool ExtTransFn(const std::string& inPort, const std::any& anyMessage);
    bool IntTransFn();
    bool OutputFn();
    TIME_T TimeAdvanceFn();
};