#pragma once
#include "DEVS/atomic_model.hpp"
#include "message.hpp"
#include "DEVS/logger.hpp"
#include "SIM/environment.hpp"
#include <utility>
#include <iostream>
#include <string>

class PlatoonLeader : public AtomicModel{
private:
    int entityId;
    std::string name;

    // Mission
    Point detPos={-1, -1};
    std::vector<Point> path;
    bool succeed = false;

    Point curPos={-1, -1};
    bool enemyDetected = false;
    bool mounted = true;

    TIME_T t_dec = 0.0f;
public:
    PlatoonLeader(Engine* engine, int entityId, std::string name);

    bool ExtTransFn(const std::string& inPort, const std::any& anyMessage);
    bool IntTransFn();
    bool OutputFn();
    TIME_T TimeAdvanceFn();
};