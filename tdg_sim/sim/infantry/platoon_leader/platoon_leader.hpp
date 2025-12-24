#pragma once
#include "DEVS/atomic_model.hpp"
#include "DEVS/logger.hpp"
#include "tdg_sim/common.hpp"
#include "tdg_sim/sim/environment/environment.hpp"
#include "platoon_pathfinding.hpp"
#include <deque>
#include <optional>

class PlatoonLeader : public AtomicModel{
private:
    int entityId; // 상부 명령 reply 용도
    std::vector<int> memberIds_;
    TaskType currentTask_ = TaskType::HOLD;
    PlatoonManeuverPlan plan_;
    std::deque<Order> pendingOrders_;
    std::optional<Order> activeOrder_;

    float t_dec = 0.0f;

    bool ActivateNextOrder(Environment& environment);
    bool IsCurrentGoalReached(Environment& environment) const;
    void ResetHoldPlan(const Order& ord);
public:
    PlatoonLeader(Engine* engine, int entityId, std::vector<int> *membersId);

    bool ExtTransFn(const std::string& inPort, const std::any& anyMessage);
    bool IntTransFn();
    bool OutputFn();
    float TimeAdvanceFn();
};
