#pragma once
#include <unordered_set>
#include "DEVS/atomic_model.hpp"
#include "common.hpp"
#include "DEVS/logger.hpp"
#include "SIM/Environment/environment.hpp"
#include "platoon_pathfinding.hpp"

class PlatoonLeader : public AtomicModel{
private:
    int entityId; // 상부 명령 reply 용도
    std::vector<int> memberIds;
    bool mounted = false;
    bool missionInProgress = false;
    std::unordered_set<int> engagedMemberIds;

    PlatoonManeuverPlan plan;

    TIME_T t_dec = 0.0f;
    TIME_T decisionInterval = 1.0f;
public:
    PlatoonLeader(Engine* engine, int entityId, std::vector<int> *membersId);

    bool ExtTransFn(const std::string& inPort, const std::any& anyMessage);
    bool IntTransFn();
    bool OutputFn();
    TIME_T TimeAdvanceFn();
};
