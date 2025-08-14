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
    int entityID;

    // Mission
    std::pair<int,int> detPos={-1, -1};
    std::vector<std::pair<int,int>> path;
    TIME_T deadline;
    bool succeed = false;

    std::pair<int,int> curPos={-1, -1};
    bool enemyDetected = false;
    bool mounted = true;

    TIME_T t_dec = 0.0f;
public:
    PlatoonLeader(Engine* engine, int entityID);

    bool ExtTransFn(const std::string& inPort, const std::any& anyMessage);
    bool IntTransFn();
    bool OutputFn();
    TIME_T TimeAdvanceFn();
};