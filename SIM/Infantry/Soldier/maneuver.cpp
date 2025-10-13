#include "maneuver.hpp"

Maneuver::Maneuver(Engine* engine, Entity* info)
    : AtomicModel(engine)
{
    this->info = info;

    this->AddState("WAIT");
    this->AddState("MOVE");
    this->AddState("DEAD");

    this->SetCurState("WAIT");

    this->AddInputPort("Order");
    this->AddInputPort("FireIn");
    this->AddOutputPort("PositionOut");

    this->LogMyBirth();
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

        logger_world << "["<< this->info->name<<"] "  << this->info->name<< " received order "<<" when Time : "<<this->engine->GetCurrentTime()<<std::endl;
        // 본인에게 온 명령인지 탐색
        auto it = message.orders.find(this->info->id);
        if (it == message.orders.end()) {
            return true;
        }
        const Order& ord = it->second;

        if(ord.task == TaskType::MOVE){
            this->nextPos = ord.to;
            this->curSpeed = 1.0f; // TODO:지형에 의존적으로 적용시킬것
            logger_world << "["<< this->info->name<<"] " 
                          << " | Task: MOVE"
                          << " | From: (" << this->info->position.x << ", " << this->info->position.y << ")"
                          << " | To:   (" << ord.to.x << ", " << ord.to.y << ")"
                          << std::endl;
            this->SetCurState("MOVE");
            this->t_mnv = mnvEquation(this->curSpeed);
        }else if(ord.task == TaskType::HOLD){
            this->nextPos = this->info->position;
            this->curSpeed = 0.0f;
            logger_world << "["<< this->info->name<<"] " 
                          << " | Task: HOLD"
                          << std::endl;
            this->SetCurState("WAIT");
            this->t_mnv = mnvEquation(this->curSpeed);
        }
    }else if(inPort == "FireIn"){  // TODO: DamageEvaluation::AM 으로 추후 분리
        FireMsg message;
        if(!TryCastMessage(anyMessage,message,"")) return false;

        // 본인에게 온 사격인지 탐색
        if( (message.senderType == ForceType::ARTILLERY && message.targetPoint == this->info->position) ||
            (message.senderType == ForceType::RIFLE && message.targetId == this->info->id) ){
                env->RequestKillEntity(this->info->id);
                this->SetCurState("DEAD");
                logger_world << "["<< this->info->name<<"] " << " is dead"<<" when Time : "<<this->engine->GetCurrentTime()<<std::endl;
        }
    }
    return true;
}

bool Maneuver::OutputFn() {
    if (this->GetCurState() == "MOVE") {
        EnvMoveResponse r = env->RequestMoveEntity(this->info->id,this->nextPos);
        if(r == EnvMoveResponse::Accepted){
            PositionMsg message;
            message.senderId = this->info->id;
            message.curPos = this->nextPos;
            std::any anyMessage = message;
            this->info->position = this->nextPos;
            logger_world << "["<< this->info->name<<"] "  << " moved to ordered position "<<" when Time : "<<this->engine->GetCurrentTime()<<std::endl;
            this->AddOutputEvent("PositionOut", anyMessage);
        }
    }
    return true;
}

bool Maneuver::IntTransFn(){
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
