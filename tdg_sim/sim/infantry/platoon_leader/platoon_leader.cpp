#include "platoon_leader.hpp"
#include <algorithm>

void PlatoonLeader::ResetHoldPlan(const Order& ord) {
    plan_ = PlatoonManeuverPlan{};
    plan_.orderedMemberIds = memberIds_;
    plan_.success = true;
    if (ord.hasDestination) {
        plan_.goal = ord.to;
        plan_.currentGoal = ord.to;
    } else {
        plan_.goal = Point{0, 0};
        plan_.currentGoal = plan_.goal;
    }
}

bool PlatoonLeader::IsCurrentGoalReached(Environment& environment) const {
    if (currentTask_ != TaskType::MOVE) {
        return true;
    }
    if (!plan_.success) {
        return false;
    }
    if (plan_.orderedMemberIds.empty()) {
        return true;
    }
    for (int memberId : plan_.orderedMemberIds) {
        auto goalIt = plan_.memberGoalPositions.find(memberId);
        if (goalIt == plan_.memberGoalPositions.end()) {
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
    while (!pendingOrders_.empty()) {
        Order ord = pendingOrders_.front();
        pendingOrders_.pop_front();

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

            plan_ = BuildPlatoonManeuverPlan(memberIds_, ord.to);
            if (!plan_.success) {
                LogSimulation(this->engine->GetCurrentTime(), this->GetNameWithId(),
                              "ORDER_FAILED", "MOVE to=", ord.to.x, ",", ord.to.y,
                              " reason=", plan_.failureReason);
                continue;
            }

            currentTask_ = TaskType::MOVE;
            activeOrder_ = ord;
            LogSimulation(this->engine->GetCurrentTime(), this->GetNameWithId(),
                          "ACTIVATE_ORDER", "task=MOVE to=", ord.to.x, ",", ord.to.y);
            return true;
        }

        if (ord.task == TaskType::HOLD) {
            currentTask_ = TaskType::HOLD;
            activeOrder_ = ord;
            ResetHoldPlan(ord);
            LogSimulation(this->engine->GetCurrentTime(), this->GetNameWithId(),
                          "ACTIVATE_ORDER", "task=HOLD");
            return true;
        }
    }

    activeOrder_.reset();
    currentTask_ = TaskType::HOLD;
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
        this->memberIds_ = *membersId;
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

        // Check if there are orders for this platoon
        auto it = message.orders.find(this->entityId);
        if (it == message.orders.end()) {
            return true;
        }

        // Clear existing pending orders and queue new ones
        // ***Input CompanyOrd replaces all previous orders***
        const auto& ordList = it->second;
        if (!ordList.empty()) {
            const std::size_t orderCount = ordList.size();
            for (int memberId : memberIds_) {
                if (memberId == this->entityId) {
                    continue;
                }
                auto& memberOrders = message.orders[memberId];
                memberOrders.reserve(memberOrders.size() + orderCount);
                memberOrders.insert(memberOrders.end(), ordList.begin(), ordList.end());
            }
        }
        pendingOrders_.clear();
        for (const Order& ord : ordList) {
            if (ord.task == TaskType::HOLD && ord.hasDestination) {
                Order moveOrder = ord;
                moveOrder.task = TaskType::MOVE;
                pendingOrders_.push_back(moveOrder);
            }
            pendingOrders_.push_back(ord);
        }
        activeOrder_.reset();
        plan_ = PlatoonManeuverPlan{};
        plan_.orderedMemberIds = memberIds_;
        plan_.success = true;
        currentTask_ = TaskType::HOLD;

        LogSimulation(this->engine->GetCurrentTime(), this->GetNameWithId(),
                      "QUEUE_ORDERS", "count=", pendingOrders_.size());

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

        for (int memberId : memberIds_) {
            markMissing(memberId);
        }
        for (int memberId : plan_.orderedMemberIds) {
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

        eraseMissingFromVec(memberIds_);
        eraseMissingFromVec(plan_.orderedMemberIds);

        for (int memberId : missing) {
            plan_.memberStartPositions.erase(memberId);
            plan_.memberGoalPositions.erase(memberId);
            plan_.memberPaths.erase(memberId);
            plan_.memberPathIndices.erase(memberId);
        }

        if (plan_.orderedMemberIds.empty()) {
            plan_.memberStartPositions.clear();
            plan_.memberGoalPositions.clear();
            plan_.memberPaths.clear();
            plan_.memberPathIndices.clear();
            plan_.success = true;
            plan_.failureReason.clear();
        }
    };

    pruneMissingMembers();

    auto ensureActiveOrder = [&]() -> bool {
        if (!activeOrder_.has_value()) {
            return ActivateNextOrder(environment);
        }
        if (currentTask_ == TaskType::MOVE && IsCurrentGoalReached(environment)) {
            return ActivateNextOrder(environment);
        }
        return activeOrder_.has_value();
    };

    if (!ensureActiveOrder()) {
        order.orders.reserve(memberIds_.size());
        for (int memberId : memberIds_) {
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

    if (this->currentTask_ == TaskType::HOLD) {
        order.orders.reserve(memberIds_.size());
        for (int memberId : memberIds_) {
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

    if (this->currentTask_ == TaskType::MOVE) {
        if (!plan_.success) {
            if (!ActivateNextOrder(environment)) {
                return true;
            }
            if (currentTask_ != TaskType::MOVE) {
                return this->OutputFn();
            }
        }

        if (plan_.orderedMemberIds.empty()) {
            return true;
        }

        order.orders.reserve(plan_.orderedMemberIds.size());

        auto alignPlanWithEnvironment = [&]() -> bool {
            for (int memberId : plan_.orderedMemberIds) {
                Point currentPos = environment.QueryEntityPosById(memberId);
                if (!environment.InBounds(currentPos)) {
                    return false;
                }

                auto pathIt = plan_.memberPaths.find(memberId);
                if (pathIt == plan_.memberPaths.end()) {
                    return false;
                }
                const std::vector<Point>& path = pathIt->second;
                if (path.empty()) {
                    plan_.memberPathIndices[memberId] = 0;
                    continue;
                }

                std::size_t idx = 0;
                auto idxIt = plan_.memberPathIndices.find(memberId);
                if (idxIt != plan_.memberPathIndices.end()) {
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
                plan_.memberPathIndices[memberId] = idx;
            }
            return true;
        };

        if (!alignPlanWithEnvironment()) {
            if (!RebuildPlatoonWayPointPlan(plan_, plan_.activeWayPoint)) {
                return true;
            }
            if (!alignPlanWithEnvironment()) {
                return true;
            }
        }

        for (int memberId : plan_.orderedMemberIds) {
            Point currentPos = environment.QueryEntityPosById(memberId);
            Order memberOrder;
            memberOrder.task = TaskType::HOLD;
            memberOrder.hasDestination = true;
            memberOrder.to = currentPos;

            auto pathIt = plan_.memberPaths.find(memberId);
            if (pathIt == plan_.memberPaths.end() || pathIt->second.empty()) {
                auto goalIt = plan_.memberGoalPositions.find(memberId);
                if (goalIt != plan_.memberGoalPositions.end()) {
                    memberOrder.to = goalIt->second;
                }
                order.orders.emplace(memberId, memberOrder);
                continue;
            }

            const std::vector<Point>& path = pathIt->second;
            std::size_t idx = 0;
            auto idxIt = plan_.memberPathIndices.find(memberId);
            if (idxIt != plan_.memberPathIndices.end()) {
                idx = idxIt->second;
            }
            if (idx >= path.size()) {
                idx = path.size() - 1;
                plan_.memberPathIndices[memberId] = idx;
            }

            if (path[idx] != currentPos) {
                auto found = std::find(path.begin(), path.end(), currentPos);
                if (found != path.end()) {
                    idx = static_cast<std::size_t>(std::distance(path.begin(), found));
                    plan_.memberPathIndices[memberId] = idx;
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
                    plan_.memberPathIndices[memberId] = idx + 1;
                }
            } else {
                auto goalIt = plan_.memberGoalPositions.find(memberId);
                if (goalIt != plan_.memberGoalPositions.end()) {
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
