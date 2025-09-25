#pragma once
#include "DEVS/atomic_model.hpp"
#include "message.hpp"
#include "DEVS/logger.hpp"
#include "SIM/Environment/environment.hpp"
#include "platoonAstar.hpp"

#include <utility>
#include <iostream>
#include <string>
#include <vector>

class PlatoonLeader : public AtomicModel{
private:
    int entityId;
    std::vector<int> *membersId;
    std::vector<Point> path;
    bool enemyDetected = false;
    bool mounted = false;

    TIME_T t_dec = 0.0f;
public:
    PlatoonLeader(Engine* engine, int entityId, std::vector<int> *membersId);

    bool ExtTransFn(const std::string& inPort, const std::any& anyMessage);
    bool IntTransFn();
    bool OutputFn();
    TIME_T TimeAdvanceFn();
};