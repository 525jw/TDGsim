#include "platoon_leader.hpp"
#include <algorithm>

void PlatoonLeader::ResetHoldPlan(const Order& ord) {
    plan = PlatoonManeuverPlan{};
    plan.orderedMemberIds = memberIds;
    plan.success = true;
    if (ord.hasDestination) {
        plan.goal = ord.to;
        plan.currentGoal = ord.to;
    } else {
        plan.goal = Point{0, 0};
        plan.currentGoal = plan.goal;
    }
}

bool PlatoonLeader::IsCurrentGoalReached(Environment& environment) const {
    if (currentTask != TaskType::MOVE) {
        return true;
    }
    if (!plan.success) {
        return false;
    }
    if (plan.orderedMemberIds.empty()) {
        return true;
    }
    for (int memberId : plan.orderedMemberIds) {
        auto goalIt = plan.memberGoalPositions.find(memberId);
        if (goalIt == plan.memberGoalPositions.end()) {
            return false;
        }
        Point currentPos = environment.QueryEntityPosById(memberId);
        if (currentPos != goalIt->second) {
            return false;
        }
    }
    return true;
}

bool PlatoonLeader::ActivateNextOrder(Environment& environment) {
    while (!pendingOrders.empty()) {
        Order ord = pendingOrders.front();
        pendingOrders.pop_front();

        if (ord.task == TaskType::BOMBARD) {
            LogSimulation(this->engine->GetCurrentTime(), this->GetNameWithId(),
                          "IGNORE_ORDER", "task=BOMBARD");
            continue;
        }

        if (ord.task == TaskType::MOVE) {
            if (!ord.hasDestination) {
                LogSimulation(this->engine->GetCurrentTime(), this->GetNameWithId(),
                              "SKIP_ORDER", "MOVE without destination");
                continue;
            }

            plan = BuildPlatoonManeuverPlan(memberIds, ord.to);
            if (!plan.success) {
                LogSimulation(this->engine->GetCurrentTime(), this->GetNameWithId(),
                              "ORDER_FAILED", "MOVE to=", ord.to.x, ",", ord.to.y,
                              " reason=", plan.failureReason);
                continue;
            }

            currentTask = TaskType::MOVE;
            activeOrder = ord;
            LogSimulation(this->engine->GetCurrentTime(), this->GetNameWithId(),
                          "ACTIVATE_ORDER", "task=MOVE to=", ord.to.x, ",", ord.to.y);
            return true;
        }

        if (ord.task == TaskType::HOLD) {
            currentTask = TaskType::HOLD;
            activeOrder = ord;
            ResetHoldPlan(ord);
            LogSimulation(this->engine->GetCurrentTime(), this->GetNameWithId(),
                          "ACTIVATE_ORDER", "task=HOLD");
            return true;
        }
    }

    activeOrder.reset();
    currentTask = TaskType::HOLD;
    Order idle;
    idle.task = TaskType::HOLD;
    idle.hasDestination = false;
    ResetHoldPlan(idle);
    return false;
}

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
    this->AddInputPort("SoldierRep");//detection res
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

        pendingOrders.clear();
        const auto& ordList = it->second;
        for (const Order& ord : ordList) {
            if (ord.task == TaskType::HOLD && ord.hasDestination) {
                Order moveOrder = ord;
                moveOrder.task = TaskType::MOVE;
                pendingOrders.push_back(moveOrder);
            }
            pendingOrders.push_back(ord);
        }
        activeOrder.reset();
        plan = PlatoonManeuverPlan{};
        plan.orderedMemberIds = memberIds;
        plan.success = true;
        currentTask = TaskType::HOLD;

        LogSimulation(this->engine->GetCurrentTime(), this->GetNameWithId(),
                      "QUEUE_ORDERS", "count=", pendingOrders.size());

        this->SetCurState("DECIDE");
        this->t_dec = 0.0f;

    } else if (inPort == "SoldierRep") {
        SoldierRep message;
        if (!TryCastMessage(anyMessage, message, "PlatoonLeader::ExtTransFn.SoldierRep")) return false;
        if (!message.enemyDetected) { // when enemyDetected==true, Soldier should be occupied by Fire exclusively
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
    if (this->GetCurState() != "DECIDE") {
        return true;
    }
    if (!EnvReady()) return true;
    Environment& environment = *env;
    PlatoonOrd order;

    auto pruneMissingMembers = [&]() {
        std::vector<int> missing;
        auto markMissing = [&](int memberId) {
            if (!environment.QueryEntityById(memberId)) {
                if (std::find(missing.begin(), missing.end(), memberId) == missing.end()) {
                    missing.push_back(memberId);
                }
            }
        };

        for (int memberId : memberIds) {
            markMissing(memberId);
        }
        for (int memberId : plan.orderedMemberIds) {
            markMissing(memberId);
        }
        if (missing.empty()) {
            return;
        }

        auto eraseMissingFromVec = [&](std::vector<int>& ids) {
            ids.erase(
                std::remove_if(
                    ids.begin(),
                    ids.end(),
                    [&](int id) {
                        return std::find(missing.begin(), missing.end(), id) != missing.end();
                    }),
                ids.end());
        };

        eraseMissingFromVec(memberIds);
        eraseMissingFromVec(plan.orderedMemberIds);

        for (int memberId : missing) {
            plan.memberStartPositions.erase(memberId);
            plan.memberGoalPositions.erase(memberId);
            plan.memberPaths.erase(memberId);
            plan.memberPathIndices.erase(memberId);
        }

        if (plan.orderedMemberIds.empty()) {
            plan.memberStartPositions.clear();
            plan.memberGoalPositions.clear();
            plan.memberPaths.clear();
            plan.memberPathIndices.clear();
            plan.success = true;
            plan.failureReason.clear();
        }
    };

    pruneMissingMembers();

    auto ensureActiveOrder = [&]() -> bool {
        if (!activeOrder.has_value()) {
            return ActivateNextOrder(environment);
        }
        if (currentTask == TaskType::MOVE && IsCurrentGoalReached(environment)) {
            return ActivateNextOrder(environment);
        }
        return activeOrder.has_value();
    };

    if (!ensureActiveOrder()) {
        order.orders.reserve(memberIds.size());
        for (int memberId : memberIds) {
            Order holdOrder;
            holdOrder.task = TaskType::HOLD;
            holdOrder.hasDestination = true;
            holdOrder.to = environment.QueryEntityPosById(memberId);
            order.orders.emplace(memberId, holdOrder);
        }

        std::any anyOrder = order;
        this->AddOutputEvent("PlatoonOrd", anyOrder);
        return true;
    }

    if (this->currentTask == TaskType::HOLD) {
        order.orders.reserve(memberIds.size());
        for (int memberId : memberIds) {
            Order holdOrder;
            holdOrder.task = TaskType::HOLD;
            holdOrder.hasDestination = true;
            holdOrder.to = environment.QueryEntityPosById(memberId);
            order.orders.emplace(memberId, holdOrder);
        }

        std::any anyOrder = order;
        this->AddOutputEvent("PlatoonOrd", anyOrder);
        return true;
    }

    if (this->currentTask == TaskType::MOVE) {
        if (!plan.success) {
            if (!ActivateNextOrder(environment)) {
                return true;
            }
            if (currentTask != TaskType::MOVE) {
                return this->OutputFn();
            }
        }

        if (plan.orderedMemberIds.empty()) {
            return true;
        }

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
            memberOrder.hasDestination = true;
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

    return true;
}

bool PlatoonLeader::IntTransFn() {
    if (this->GetCurState() == "DECIDE") {
        this->SetCurState("WAIT");
        this->t_dec = 0.0f;
    }
    return true;
}

float PlatoonLeader::TimeAdvanceFn() {
    if (this->GetCurState() == "WAIT") return TIME_INF;
    if (this->GetCurState() == "DECIDE") return t_dec;
    return -1;
}
