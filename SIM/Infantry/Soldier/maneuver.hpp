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
    TIME_T lookAroundPeriod = 1.5f; // TODO:이산시간시뮬레이션화 될 수 있음, env와 pos가 상호작용하게 하는 편이 좋을 것

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