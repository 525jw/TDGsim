#include "platoon_leader.hpp"
#include <cmath>

PlatoonLeader::PlatoonLeader(Engine* engine, int entityId, std::vector<int> *membersId)
    : AtomicModel(engine)
{
    this->entityId = entityId;
    this->memberIds = membersId;

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
        if (!TryCastMessage(anyMessage, message, "")) return false;

        // Check whether this platoon is the intended recipient
        auto it = message.orders.find(this->entityId);
        if (it == message.orders.end()) {
            return true;
        }
        const Order& ord = it->second;

        if (ord.task == TaskType::MOVE) {
            this->plan = BuildPlatoonManeuverPlan(*memberIds, ord.to);
        } else if (ord.task == TaskType::HOLD) {
            PlatoonManeuverPlan holdPlan;
            holdPlan.success = true;
            holdPlan.failureReason.clear();
            holdPlan.orderedMemberIds = *memberIds;

            long long sumx = 0, sumy = 0;
            for (int id : holdPlan.orderedMemberIds) {
                Point p = env->QueryEntityPosById(id);
                holdPlan.memberStartPositions[id] = p;
                holdPlan.memberPaths[id] = {}; // HOLD: no movement required
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
        }
        this->SetCurState("DECIDE");
    } else if (inPort == "SoldierRep") {
        SoldierRep message;
        if (!TryCastMessage(anyMessage, message, "")) return false;
        this->enemyDetected = message.enemyDetected;
        if (this->enemyDetected) {
            this->SetCurState("DECIDE");
        }
    }
    return true;
}

bool PlatoonLeader::OutputFn() {
    if (this->GetCurState() == "DECIDE") {
        PlatoonOrd order; // 명령
        bool allMembersAtGoal = true;
        if (enemyDetected) {
            if (!EnvReady()) return true;
            order.orders.reserve(memberIds->size());
            Environment& environment = *env;
            for (int memberId : *memberIds) {
                Order holdOrder;
                holdOrder.task = TaskType::HOLD;
                holdOrder.to = environment.QueryEntityPosById(memberId);
                order.orders.emplace(memberId, holdOrder);
            }
            allMembersAtGoal = false;
        } else {
            if (!plan.success) return true;
            if (!EnvReady()) return true;
            order.orders.reserve(plan.orderedMemberIds.size());
            Environment& environment = *env;
            for (int memberId : plan.orderedMemberIds) {
                Order memberOrder;
                Point currentPos = environment.QueryEntityPosById(memberId);
                memberOrder.task = TaskType::HOLD;
                memberOrder.to = currentPos;

                auto pathIt = plan.memberPaths.find(memberId);
                if (pathIt != plan.memberPaths.end()) {
                    const std::vector<Point>& path = pathIt->second;
                    Point nextTarget = plan.goal;
                    bool needsMove = false;

                    if (!path.empty()) {
                        auto posIt = std::find(path.begin(), path.end(), currentPos);
                        if (posIt == path.end()) {
                            nextTarget = path.front();
                            needsMove = (currentPos != nextTarget);
                        } else {
                            ++posIt;
                            if (posIt != path.end()) {
                                nextTarget = *posIt;
                                needsMove = (currentPos != nextTarget);
                            } else {
                                nextTarget = plan.goal;
                                needsMove = (currentPos != nextTarget);
                            }
                        }
                    } else {
                        nextTarget = plan.goal;
                        needsMove = (currentPos != nextTarget);
                    }

                    if (needsMove) {
                        memberOrder.task = TaskType::MOVE;
                        memberOrder.to = nextTarget;
                        allMembersAtGoal = false;
                    }
                }

                order.orders.emplace(memberId, memberOrder);
            }
        }
        std::any anyOrder = order;
        this->AddOutputEvent("PlatoonOrd", anyOrder);

        PlatoonRep report; // 보고
        report.entityId = this->entityId;
        report.succeed = (!enemyDetected) && plan.success && allMembersAtGoal;

        std::any anyReport = report;
        this->AddOutputEvent("PlatoonRep", anyReport);
    }
    return true;
}

bool PlatoonLeader::IntTransFn() {
    if (this->GetCurState() == "DECIDE") {
        this->SetCurState("WAIT");
    }
    return true;
}

TIME_T PlatoonLeader::TimeAdvanceFn() {
    if (this->GetCurState() == "WAIT") return TIME_INF;
    if (this->GetCurState() == "DECIDE") return t_dec;
    return -1;
}
