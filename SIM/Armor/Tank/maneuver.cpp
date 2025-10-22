#include "maneuver.hpp"

TankManeuver::TankManeuver(Engine* engine, Entity* info)
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

}

TIME_T TankManeuver::mnvEquation(float speed) {
    if(speed){
        return 1.0f / speed;
    }else{
        return this->lookAroundPeriod;
    }
}

bool TankManeuver::ExtTransFn(const std::string& inPort, const std::any& anyMessage) {
    if (inPort == "Order") {
        PlatoonOrd message;
        if(!TryCastMessage(anyMessage,message,"")) return false;
        // 본인에게 온 명령인지 탐색
        auto it = message.orders.find(this->info->id);
        if (it == message.orders.end()) {
            return true;
        }
        const Order& ord = it->second;

        if(ord.task == TaskType::MOVE){ // 이동명령
            this->nextPos = ord.to;
            this->curSpeed = moveSpeed; // tank speed
            LogSimulation(this->engine->GetCurrentTime(),this->GetName(),"RECEIVE_ORDER",
                        "task=","MOVE",
                        " from=(",this->info->position.x,", ",this->info->position.y,")",
                        " to=(",ord.to.x,", ",ord.to.y,")");
        }else if(ord.task == TaskType::HOLD){ // 정지명령
            this->nextPos = this->info->position;
            this->curSpeed = 0.0f;
            LogSimulation(this->engine->GetCurrentTime(),this->GetName(),"RECEIVE_ORDER","task=","HOLD");
        }

        this->t_mnv = mnvEquation(this->curSpeed);
        if(GetCurState()=="MOVE") this->t_mnv = std::max(0.f, t_mnv - executedTime);
        this->SetCurState("MOVE");
    }else if(inPort == "FireIn"){  // TODO: DamageEvaluation::AM 으로 추후 분리
        FireMsg message;
        if(!TryCastMessage(anyMessage,message,"")) return false;

        // 본인에게 온 사격인지 탐색 + 확률 적용
        bool targeted = false;
        float pKill = 0.0f;
        if (message.senderType == ForceType::ARTILLERY && message.targetPoint == this->info->position){
            targeted = true;
            pKill = pKillFromArtillery;
        } else if (message.senderType == ForceType::RIFLE && message.targetId == this->info->id){
            targeted = true;
            pKill = pKillFromRifle;
        } else if (message.senderType == ForceType::TANK && message.targetId == this->info->id){
            targeted = true;
            pKill = pKillFromTank;
        }

        if(targeted){
            ensureRng();
            std::uniform_real_distribution<float> dist(0.0f, 1.0f);
            if (dist(rng) <= pKill) {
                env->RequestKillEntity(this->info->id);
                this->SetCurState("DEAD");
                LogSimulation(this->engine->GetCurrentTime(),this->GetName(),"DEAD");
            }
        }
    }
    return true;
}

bool TankManeuver::OutputFn() {
    if (this->GetCurState() == "MOVE") {
        EnvMoveResponse r = env->RequestMoveEntity(this->info->id,this->nextPos);
        if(r == EnvMoveResponse::Accepted){
            PositionMsg message;
            message.senderId = this->info->id;
            message.curPos = this->nextPos;
            std::any anyMessage = message;
            if(this->info->position != this->nextPos){
                LogSimulation(this->engine->GetCurrentTime(),this->GetName(),"MOVE",
                        " from=(",this->info->position.x,", ",this->info->position.y,")",
                        " to=(",this->nextPos.x,", ",this->nextPos.y,")");
            }else{
                LogSimulation(this->engine->GetCurrentTime(),this->GetName(),"HOLD_POSITION");
            }
            
            this->info->position = this->nextPos; 
            this->AddOutputEvent("PositionOut", anyMessage);
        }
    }else if(this->GetCurState()=="DEAD"){
        
    }
    return true;
}

bool TankManeuver::IntTransFn(){
    if (this->GetCurState() == "MOVE") {
        this->SetCurState("WAIT");
    }
    return true;
}

TIME_T TankManeuver::TimeAdvanceFn() {
    if (this->GetCurState() == "WAIT") return TIME_INF;
    if (this->GetCurState() == "MOVE") return t_mnv;
    if (this->GetCurState() == "DEAD") return TIME_INF;
    return -1;
}

