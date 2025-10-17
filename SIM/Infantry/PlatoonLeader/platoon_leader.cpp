#include "platoon_leader.hpp"
#include <algorithm>

PlatoonLeader::PlatoonLeader(Engine* engine, int entityId, std::vector<int> *membersId)
    : AtomicModel(engine)
{
    this->entityId = entityId;
    if (membersId) {
        this->memberIds = *membersId;
    }

    this->AddState("WAIT");
    this->AddState("DECIDE"); // DECIDE and ORDER

    this->SetCurState("WAIT");

    this->AddInputPort("CompanyOrd");
    this->AddInputPort("SoldierRep");
    this->AddInputPort("FireFinished");

    this->AddOutputPort("PlatoonOrd");
    this->AddOutputPort("PlatoonRep");

}

bool PlatoonLeader::ExtTransFn(const std::string& inPort, const std::any& anyMessage) {
    if (inPort == "CompanyOrd") {
        CompanyOrd message;
        if (!TryCastMessage(anyMessage, message, "PlatoonLeader::ExtTransFn.CompanyOrd")) return false;

        auto it = message.orders.find(this->entityId);
        if (it == message.orders.end()) {
            return true;
        }
        const Order& ord = it->second;

        this->currentTask = ord.task;
        if (ord.task == TaskType::MOVE) {
            this->plan = BuildPlatoonManeuverPlan(memberIds, ord.to);
        } else if (ord.task == TaskType::HOLD) {
            this->plan = PlatoonManeuverPlan{};
            this->plan.orderedMemberIds = memberIds;
            this->plan.goal = ord.to;
            this->plan.currentGoal = ord.to;
            this->plan.success = true;
        } else {
            this->plan = PlatoonManeuverPlan{};
        }
        this->SetCurState("DECIDE");
        this->t_dec = 0.0f;

    } else if (inPort == "SoldierRep") {
        SoldierRep message;
        if (!TryCastMessage(anyMessage, message, "PlatoonLeader::ExtTransFn.SoldierRep")) return false;
        if (!message.enemyDetected) {
            this->SetCurState("DECIDE");
            this->t_dec = 0.0f;
        }
    } else if (inPort == "FireFinished"){
        this->SetCurState("DECIDE");
        this->t_dec = 0.0f;
    }
    return true;
}

bool PlatoonLeader::OutputFn() {
    if (this->GetCurState() == "DECIDE") {
        if (!EnvReady()) return true;
        Environment& environment = *env;
        PlatoonOrd order;

        if (this->currentTask == TaskType::HOLD) {
            order.orders.reserve(memberIds.size());
            for (int memberId : memberIds) {
                Order holdOrder;
                holdOrder.task = TaskType::HOLD;
                holdOrder.to = environment.QueryEntityPosById(memberId);
                order.orders.emplace(memberId, holdOrder);
            }

            std::any anyOrder = order;
            this->AddOutputEvent("PlatoonOrd", anyOrder);
            return true;
        }

        if (this->currentTask == TaskType::MOVE) {
            if (!plan.success) return true;
            order.orders.reserve(plan.orderedMemberIds.size());

            auto alignPlanWithEnvironment = [&]() -> bool {
                for (int memberId : plan.orderedMemberIds) {
                    Point currentPos = environment.QueryEntityPosById(memberId);
                    if (!environment.InBounds(currentPos)) {
                        return false;
                    }

                    auto pathIt = plan.memberPaths.find(memberId);
                    if (pathIt == plan.memberPaths.end()) {
                        return false;
                    }
                    const std::vector<Point>& path = pathIt->second;
                    if (path.empty()) {
                        plan.memberPathIndices[memberId] = 0;
                        continue;
                    }

                    std::size_t idx = 0;
                    auto idxIt = plan.memberPathIndices.find(memberId);
                    if (idxIt != plan.memberPathIndices.end()) {
                        idx = idxIt->second;
                        if (idx >= path.size()) {
                            idx = path.size() - 1;
                        }
                    }

                    if (path[idx] != currentPos) {
                        auto found = std::find(path.begin(), path.end(), currentPos);
                        if (found == path.end()) {
                            return false;
                        }
                        idx = static_cast<std::size_t>(std::distance(path.begin(), found));
                    }
                    plan.memberPathIndices[memberId] = idx;
                }
                return true;
            };

            if (!alignPlanWithEnvironment()) {
                if (!RebuildPlatoonWaypointPlan(plan, plan.activeWaypoint)) {
                    return true;
                }
                if (!alignPlanWithEnvironment()) {
                    return true;
                }
            }

            for (int memberId : plan.orderedMemberIds) {
                Point currentPos = environment.QueryEntityPosById(memberId);
                Order memberOrder;
                memberOrder.task = TaskType::HOLD;
                memberOrder.to = currentPos;

                auto pathIt = plan.memberPaths.find(memberId);
                if (pathIt == plan.memberPaths.end() || pathIt->second.empty()) {
                    auto goalIt = plan.memberGoalPositions.find(memberId);
                    if (goalIt != plan.memberGoalPositions.end()) {
                        memberOrder.to = goalIt->second;
                    }
                    order.orders.emplace(memberId, memberOrder);
                    continue;
                }

                const std::vector<Point>& path = pathIt->second;
                std::size_t idx = 0;
                auto idxIt = plan.memberPathIndices.find(memberId);
                if (idxIt != plan.memberPathIndices.end()) {
                    idx = idxIt->second;
                }
                if (idx >= path.size()) {
                    idx = path.size() - 1;
                    plan.memberPathIndices[memberId] = idx;
                }

                if (path[idx] != currentPos) {
                    auto found = std::find(path.begin(), path.end(), currentPos);
                    if (found != path.end()) {
                        idx = static_cast<std::size_t>(std::distance(path.begin(), found));
                        plan.memberPathIndices[memberId] = idx;
                    } else {
                        order.orders.emplace(memberId, memberOrder);
                        continue;
                    }
                }

                if (idx + 1 < path.size()) {
                    Point nextTarget = path[idx + 1];
                    if (nextTarget != currentPos) {
                        memberOrder.task = TaskType::MOVE;
                        memberOrder.to = nextTarget;
                        plan.memberPathIndices[memberId] = idx + 1;
                    }
                } else {
                    auto goalIt = plan.memberGoalPositions.find(memberId);
                    if (goalIt != plan.memberGoalPositions.end()) {
                        memberOrder.to = goalIt->second;
                    }
                }

                order.orders.emplace(memberId, memberOrder);
            }

            std::any anyOrder = order;
            this->AddOutputEvent("PlatoonOrd", anyOrder);
            return true;
        }
    }
    return true;
}

bool PlatoonLeader::IntTransFn() {
    if (this->GetCurState() == "DECIDE") {
        this->SetCurState("WAIT");
        this->t_dec = 0.0f;
    }
    return true;
}

TIME_T PlatoonLeader::TimeAdvanceFn() {
    if (this->GetCurState() == "WAIT") return TIME_INF;
    if (this->GetCurState() == "DECIDE") return t_dec;
    return -1;
}
