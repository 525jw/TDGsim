#include "platoon_leader.hpp"

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

            auto nextStepFor = [&](Point p) -> Point {
                if (plan.gridWidth <= 0 || plan.gridHeight <= 0) return Point{-1, -1};
                if (p.x < 0 || p.y < 0) return Point{-1, -1};
                if (p.x >= plan.gridWidth || p.y >= plan.gridHeight) return Point{-1, -1};
                std::size_t idx =
                    static_cast<std::size_t>(p.y) * static_cast<std::size_t>(plan.gridWidth) +
                    static_cast<std::size_t>(p.x);
                if (idx >= plan.nextStepGrid.size()) return Point{-1, -1};
                return plan.nextStepGrid[idx];
            };

            for (int memberId : plan.orderedMemberIds) {
                Point currentPos = environment.QueryEntityPosById(memberId);
                Order memberOrder;
                memberOrder.task = TaskType::HOLD;
                memberOrder.to = currentPos;

                Point nextTarget = nextStepFor(currentPos);
                if (nextTarget.x != -1 && nextTarget.y != -1 && nextTarget != currentPos) {
                    memberOrder.task = TaskType::MOVE;
                    memberOrder.to = nextTarget;
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
