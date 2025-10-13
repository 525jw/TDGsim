#include "platoon_leader.hpp"
#include <cmath>
#include <new>

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

    this->LogMyBirth();
}

bool PlatoonLeader::ExtTransFn(const std::string& inPort, const std::any& anyMessage) {
    if (inPort == "CompanyOrd") {
        CompanyOrd message;
        if (!TryCastMessage(anyMessage, message, "")) return false;

        // Check whether this platoon is the intended recipient
        logger_world << "[TMP] " <<this->entityId<< " received order "<<" when Time : "<<this->engine->GetCurrentTime()<<std::endl;
        auto it = message.orders.find(this->entityId);
        if (it == message.orders.end()) {
            return true;
        }
        const Order& ord = it->second;

        if (ord.task == TaskType::MOVE) {
            try {
                if (EnvReady()) {
                    Environment& environment = *env;
                    logger_world << "[DEBUG]" << " Environment size for plan " << environment.GetWidth()
                                 << "x" << environment.GetHeight() << " members "
                                 << memberIds.size() << std::endl;
                }
                this->plan = BuildPlatoonManeuverPlan(memberIds, ord.to);
            } catch (const std::bad_alloc&) {
                logger_world << "[ERROR]" << " Maneuver plan allocation failed for platoon " << this->entityId
                             << " at time " << this->engine->GetCurrentTime() << std::endl;
                throw;
            }
            logger_world << "[TMP] " <<this->entityId<< " built maneuver plan "<<" when Time : "<<this->engine->GetCurrentTime()<<std::endl;
            missionInProgress = plan.success;
        } else if (ord.task == TaskType::HOLD) {
            PlatoonManeuverPlan holdPlan;
            holdPlan.success = true;
            holdPlan.failureReason.clear();
            holdPlan.orderedMemberIds = memberIds;

            long long sumx = 0, sumy = 0;
            for (int id : holdPlan.orderedMemberIds) {
                Point p = env->QueryEntityPosById(id);
                holdPlan.memberStartPositions[id] = p;
                sumx += p.x;
                sumy += p.y;
            }
            if (!holdPlan.orderedMemberIds.empty()) {
                Point ref{
                    int(sumx / static_cast<long long>(holdPlan.orderedMemberIds.size())),
                    int(sumy / static_cast<long long>(holdPlan.orderedMemberIds.size()))
                };
                holdPlan.referenceStart = ref;
                holdPlan.goal = ref; // use the centroid as the hold position
            }
            this->plan = std::move(holdPlan);
            missionInProgress = false;
        }

        this->SetCurState("DECIDE");
        this->t_dec = 0.0f;
        logger_world << "[TMP] " <<this->entityId<< " state : DECIDE "<<" when Time : "<<this->engine->GetCurrentTime()<<std::endl;

    } else if (inPort == "SoldierRep") {
        SoldierRep message;
        if (!TryCastMessage(anyMessage, message, "")) return false;
        if (message.enemyDetected) {
            engagedMemberIds.insert(message.entityId);
        } else {
            engagedMemberIds.erase(message.entityId);
        }
        this->SetCurState("DECIDE");
        this->t_dec = 0.0f;
        missionInProgress = true;
    }
    return true;
}

bool PlatoonLeader::OutputFn() {
    if (this->GetCurState() == "DECIDE") {
        logger_world << "[TMP] " <<this->entityId<< " decide "<<" when Time : "<<this->engine->GetCurrentTime()<<std::endl;
        PlatoonOrd order; // 명령
        bool allMembersAtGoal = true;
        const bool enemyDetected = !engagedMemberIds.empty();
        bool needFollowup = false;

        if (enemyDetected) {
            if (!EnvReady()) {
                missionInProgress = false;
                return true;
            }
            order.orders.reserve(memberIds.size());
            Environment& environment = *env;
            for (int memberId : memberIds) {
                Order holdOrder;
                holdOrder.task = TaskType::HOLD;
                holdOrder.to = environment.QueryEntityPosById(memberId);
                order.orders.emplace(memberId, holdOrder);
            }
            allMembersAtGoal = false;
            needFollowup = true;
        } else {
            if (!plan.success) {
                missionInProgress = false;
                return true;
            }
            if (!EnvReady()) {
                missionInProgress = false;
                return true;
            }
            order.orders.reserve(plan.orderedMemberIds.size());
            Environment& environment = *env;
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
                Order memberOrder;
                Point currentPos = environment.QueryEntityPosById(memberId);
                memberOrder.task = TaskType::HOLD;
                memberOrder.to = currentPos;

                if (currentPos != plan.goal) {
                    Point nextTarget = nextStepFor(currentPos);
                    if (nextTarget.x != -1 && nextTarget.y != -1 && nextTarget != currentPos) {
                        memberOrder.task = TaskType::MOVE;
                        memberOrder.to = nextTarget;
                    }
                }

                if (memberOrder.task == TaskType::MOVE) {
                    needFollowup = true;
                    allMembersAtGoal = false;
                } else if (currentPos != plan.goal) {
                    needFollowup = true;
                    allMembersAtGoal = false;
                }
                order.orders.emplace(memberId, memberOrder);
            }
        }
        std::any anyOrder = order;
        this->AddOutputEvent("PlatoonOrd", anyOrder);
        logger_world << "[TMP] " <<this->entityId<< " ordered "<<" when Time : "<<this->engine->GetCurrentTime()<<std::endl;

        const bool missionSuccess = (!enemyDetected) && plan.success && allMembersAtGoal;
        const bool shouldReport = enemyDetected || missionSuccess;
        if (shouldReport) {
            PlatoonRep report; // 보고
            report.entityId = this->entityId;
            report.succeed = missionSuccess;

            std::any anyReport = report;
            this->AddOutputEvent("PlatoonRep", anyReport);
        }

        missionInProgress = needFollowup;
    }
    return true;
}

bool PlatoonLeader::IntTransFn() {
    if (this->GetCurState() == "DECIDE") {
        if (missionInProgress) {
            this->SetCurState("DECIDE");
            this->t_dec = decisionInterval;
        } else {
            this->SetCurState("WAIT");
            this->t_dec = 0.0f;
        }
    }
    return true;
}

TIME_T PlatoonLeader::TimeAdvanceFn() {
    if (this->GetCurState() == "WAIT") return TIME_INF;
    if (this->GetCurState() == "DECIDE") return t_dec;
    return -1;
}
