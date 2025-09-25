#include "fire.hpp"
#include <cmath>

Fire::Fire(Engine* engine, int entityId, Entity* info)
    : AtomicModel(engine)
{
    this->entityId = entityId;
    this->info = info;

    this->AddState("WAIT");
    this->AddState("FIRE");

    this->SetCurState("WAIT");

    this->AddInputPort("SoldierRep");
    this->AddOutputPort("FireOut");
}

TIME_T Fire::fireEquation(){
    return this->fireFreq;
}

bool Fire::ExtTransFn(const std::string& inPort, const std::any& anyMessage) {
    if (inPort == "SoldierRep" && this->GetCurState()=="WAIT") {
        SoldierRep message;
        if(!TryCastMessage(anyMessage,message,"")) return false;
        if(message.enemyDetected && !message.enemyIds){
            ensureRng();
            std::uniform_int_distribution<size_t> dist(0, message.enemyIds->size() - 1);
            targetId = (*message.enemyIds)[dist(rng)];

            this->SetCurState("FIRE");
        }else{
            targetId = -1;
            this->SetCurState("WAIT");
        }
    }
    return true;
}

bool Fire::OutputFn(){
    if (this->GetCurState() == "FIRE"){
        this->t_fire = this->fireEquation();

        ensureRng();
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        float roll = dist(rng);
        if (roll<=this->accuracy){
            FireMsg message;
            message.senderId = this->entityId;
            message.senderType = this->info->forceType;
            message.targetId = this->targetId;
            std::any anyMessage = message;
            this->AddOutputEvent("detRes",anyMessage);
        }
        else{
            // 빗나가면 메시지 전송 X, 추후 FireMsg.hit=false 방식 도입
        }
    }
    return true;
}

bool Fire::IntTransFn(){
    if (this->GetCurState() == "FIRE")
        this->SetCurState("WAIT");
    return true;
}

TIME_T Fire::TimeAdvanceFn(){
    if (this->GetCurState() == "WAIT") return TIME_INF;
    if (this->GetCurState() == "FIRE") return t_fire;

    return -1;
}