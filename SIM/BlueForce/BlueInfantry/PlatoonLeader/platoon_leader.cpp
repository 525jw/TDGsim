#include "platoon_leader.hpp"
#include <cmath>

PlatoonLeader::PlatoonLeader(Engine* engine, int entityId, std::vector<int> *membersId)
    : AtomicModel(engine)
{
    this->entityId;
    this->membersId=membersId;
    this->AddState("WAIT");
    this->AddState("DECIDE");

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

        // 본인에게 온 명령인지 탐색
        auto it = message.orders.find(this->entityId);
        if (it == message.orders.end()) {
            return true;
        }

        const Order& ord = it->second;
        if(ord.task==TaskType::MOVE){
            // 본인에게 온 명령이면 경로탐색 결과 vector에 담기
            Point cur{-1, -1}; 
            if(this->membersId && this->membersId->empty()){ 
                cur = ENV->QueryEntityPosById(this->membersId->back());
            }
            this->path = AStarPlan(cur, ord.to);
        }
        this->SetCurState("DECIDE");
    } else if (inPort == "SoldierRep") {
        SoldierRep message;
        if (!TryCastMessage(anyMessage,message,"")) return false;
        this->enemyDetected = message.enemyDetected;
        if(this->enemyDetected){
            this->SetCurState("DECIDE");
        }
    }
    return true;
}
bool PlatoonLeader::OutputFn() {
    if (this->GetCurState() == "DECIDE") {
        PlatoonOrd order;
        if(enemyDetected){
            // off vehicle
            // deploy all entity(BFS)
        }else{
            // on vehicle
            // path 하나씩 pop
        }
        std::any anyOrder = order;
        this->AddOutputEvent("PlatoonOrd", anyOrder);
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
    else if (this->GetCurState() == "DECIDE") return t_dec;
    return -1;
}