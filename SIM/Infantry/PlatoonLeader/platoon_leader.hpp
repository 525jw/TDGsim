#pragma once
#include "DEVS/atomic_model.hpp"
#include "common.hpp"
#include "DEVS/logger.hpp"
#include "SIM/Environment/environment.hpp"
#include "platoon_pathfinding.hpp"
#include <deque>
#include <optional>

class PlatoonLeader : public AtomicModel{
private:
    int entityId; // 상부 명령 reply 용도
    std::vector<int> memberIds;
    TaskType currentTask = TaskType::HOLD;
    PlatoonManeuverPlan plan;
    std::deque<Order> pendingOrders;
    std::optional<Order> activeOrder;

    TIME_T t_dec = 0.0f;

    bool ActivateNextOrder(Environment& environment);
    bool IsCurrentGoalReached(Environment& environment) const;
    void ResetHoldPlan(const Order& ord);
public:
    PlatoonLeader(Engine* engine, int entityId, std::vector<int> *membersId);

    bool ExtTransFn(const std::string& inPort, const std::any& anyMessage);
    bool IntTransFn();
    bool OutputFn();
    TIME_T TimeAdvanceFn();
};
