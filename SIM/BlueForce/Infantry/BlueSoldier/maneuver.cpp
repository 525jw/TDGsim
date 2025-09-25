#include "maneuver.hpp"
#include <cmath>

Maneuver::Maneuver(Engine* engine, int entityId, Entity* info)
    : AtomicModel(engine)
{
    this->entityId=entityId;
    this->info=info;

    this->AddState("WAIT");
    this->AddState("MOVE");
    this->AddState("DEAD");

    this->SetCurState("WAIT");

    this->AddInputPort("PlatoonOrd");
    this->AddInputPort("FireIn");
    this->AddOutputPort("PositionOut");
}

TIME_T Maneuver::mnvEquation(float speed) {
    return 1.0f / speed;
}

bool Maneuver::ExtTransFn(const std::string& inPort, const std::any& anyMessage) {
    if (inPort == "PlatoonOrd" && this->GetCurState()=="WAIT") {
        PlatoonOrd message;
        if(!TryCastMessage(anyMessage,message,"")) return false;

        // 본인에게 온 명령인지 탐색
        auto it = message.orders.find(this->entityId);
        if (it == message.orders.end()) {
            return true;
        }
        const Order& ord = it->second;
        this->nextPos = ord.to;
        this->curSpeed = 1.0f; // 지형에 의존적으로 적용시킬것

        this->SetCurState("MOVE");
    }else if(inPort == "FireIn"){
        FireMsg message;
        if(!TryCastMessage(anyMessage,message,"")) return false;

        // 본인에게 온 사격인지 탐색
        if( (message.senderType == ForceType::ARTILLERY && message.targetPoint == this->info->position) ||
            (message.senderType == ForceType::RIFLE && message.targetId == this->entityId) ){
                ENV->RequestKillEntity(this->entityId);
                this->SetCurState("DEAD");
        }
    }
    return true;
}

bool Maneuver::OutputFn() {
    if (this->GetCurState() == "MOVE") {
        this->t_mnv = mnvEquation(this->curSpeed);
        EnvMoveResponse r = ENV->RequestMoveEntity(this->entityId,this->nextPos);
        if(r == EnvMoveResponse::OccupiedOther){
            t_mnv*=2; // 기다리기
        }else if(r == EnvMoveResponse::Accepted){
            PositionMsg message;
            message.senderId = this->entityId;
            message.curPos = this->nextPos;
            std::any anyMessage = message;
            this->info->position = this->nextPos;
            this->AddOutputEvent("PositionOut", anyMessage); // NOTE : Position 정보는 이미 바뀌어 있음(Soldier & Environment), Ack 용도
        }
    }
    return true;
}

bool Maneuver::IntTransFn() {
    if (this->GetCurState() == "MOVE") {
        this->SetCurState("WAIT");
    }
    return true;
}

TIME_T Maneuver::TimeAdvanceFn() {
    if (this->GetCurState() == "WAIT") return TIME_INF;
    if (this->GetCurState() == "MOVE") return t_mnv;
    if (this->GetCurState() == "DEAD") return TIME_INF;
    return -1;
}