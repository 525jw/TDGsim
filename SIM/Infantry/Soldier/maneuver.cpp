#include "maneuver.hpp"

Maneuver::Maneuver(Engine* engine, int entityId, Entity* info)
    : AtomicModel(engine)
{
    this->entityId=entityId;
    this->info = info;

    this->AddState("WAIT");
    this->AddState("MOVE");
    this->AddState("DEAD");

    this->SetCurState("WAIT");

    this->AddInputPort("Order");
    this->AddInputPort("FireIn");
    this->AddOutputPort("PositionOut");
}

TIME_T Maneuver::mnvEquation(float speed) {\
    if(speed){
        return 1.0f / speed;
    }else{
        return TIME_INF;
    }
}

bool Maneuver::ExtTransFn(const std::string& inPort, const std::any& anyMessage) {
    if (inPort == "Order") {
        PlatoonOrd message;
        if(!TryCastMessage(anyMessage,message,"")) return false;

        // 본인에게 온 명령인지 탐색
        auto it = message.orders.find(this->entityId);
        if (it == message.orders.end()) {
            return true;
        }
        const Order& ord = it->second;

        if(ord.task == TaskType::MOVE){
            this->nextPos = ord.to;
            this->curSpeed = 1.0f; // TODO:지형에 의존적으로 적용시킬것
            this->SetCurState("MOVE");
        }else if(ord.task == TaskType::HOLD){
            this->curSpeed = 0.0;
            this->SetCurState("WAIT");
        }
        this->t_mnv = mnvEquation(this->curSpeed);
    }else if(inPort == "FireIn"){  // TODO: DamageEvaluation::AM 으로 추후 분리
        FireMsg message;
        if(!TryCastMessage(anyMessage,message,"")) return false;

        // 본인에게 온 사격인지 탐색
        if( (message.senderType == ForceType::ARTILLERY && message.targetPoint == this->info->position) ||
            (message.senderType == ForceType::RIFLE && message.targetId == this->entityId) ){
                env->RequestKillEntity(this->entityId);
                this->SetCurState("DEAD");
                logger_world << "["<<this->info->name<<"] "<< "is dead"<<" when Time : "<<this->engine->GetCurrentTime()<<std::endl;
        }
    }
    return true;
}

bool Maneuver::OutputFn() {
    if (this->GetCurState() == "MOVE") {
        EnvMoveResponse r = env->RequestMoveEntity(this->entityId,this->nextPos);
        if(r == EnvMoveResponse::Accepted){
            PositionMsg message;
            message.senderId = this->entityId;
            message.curPos = this->nextPos;
            std::any anyMessage = message;
            this->info->position = this->nextPos;
            this->AddOutputEvent("PositionOut", anyMessage);
        }
    }
    return true;
}

TIME_T Maneuver::TimeAdvanceFn() {
    if (this->GetCurState() == "WAIT") return TIME_INF;
    if (this->GetCurState() == "MOVE") return t_mnv;
    if (this->GetCurState() == "DEAD") return TIME_INF;
    return -1;
}