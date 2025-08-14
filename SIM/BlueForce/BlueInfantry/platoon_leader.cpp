#include "platoon_leader.hpp"
#include "pathfinder.cpp"
#include <cmath>

PlatoonLeader::PlatoonLeader(Engine* engine, int entityID)
    : AtomicModel(engine)
{
    this->entityID = entityID;

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

        const int id = this->entityID; // 본인에게 온 명령인지 탐색
        auto it = message.orders.find(id);
        if (it != message.orders.end()) { // 본인에게 온 명령이면
            this->detPos   = it->second.detPos;
            this->deadline = it->second.deadline;
            // 경로탐색 결과 vector에 담기
            // this->path = Astar();
            if(this->GetCurState()=="WAIT"){
                this->SetCurState("DECIDE");
            }
        }
    } else if (inPort == "SoldierRep") {
        SoldierRep message;
        if (!TryCastMessage(anyMessage,message,"")) return false;

        this->enemyDetected = message.enemyDetected;

        if(this->GetCurState()=="WAIT"){
            this->SetCurState("DECIDE");
        }
    }
    return true;
}

bool PlatoonLeader::IntTransFn() {
    if (this->GetCurState() == "DECIDE") {
        this->SetCurState("WAIT");
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

TIME_T PlatoonLeader::TimeAdvanceFn() {
    if (this->GetCurState() == "WAIT") return TIME_INF;
    else if (this->GetCurState() == "DECIDE") return t_dec;
    return -1;
}