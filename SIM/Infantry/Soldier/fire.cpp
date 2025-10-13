#include "fire.hpp"

Fire::Fire(Engine* engine, Entity* info)
    : AtomicModel(engine)
{
    this->info = info;

    this->AddState("WAIT");
    this->AddState("FIRE");

    this->SetCurState("WAIT");

    this->AddInputPort("SoldierRep");
    this->AddOutputPort("FireOut");

    this->LogMyBirth();
}

TIME_T Fire::fireEquation(){
    return this->fireFreq;
}

bool Fire::ExtTransFn(const std::string& inPort, const std::any& anyMessage) {
    if (inPort == "SoldierRep" && this->GetCurState()=="WAIT") {
        SoldierRep message;
        if(!TryCastMessage(anyMessage,message,"")) return false;
        if (message.enemyDetected && message.enemyIds && !message.enemyIds->empty()) {
            ensureRng();
            std::uniform_int_distribution<size_t> dist(0, message.enemyIds->size() - 1);
            targetId = (*message.enemyIds)[dist(rng)];

            this->SetCurState("FIRE");
            this->t_fire = this->fireEquation();
        }else{
            targetId = -1;
            this->SetCurState("WAIT");
        }
    }
    return true;
}

bool Fire::OutputFn(){
    if (this->GetCurState() != "FIRE") {
        return true;
    }

    ensureRng();
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    float roll = dist(rng);
    if (roll <= this->accuracy) {
        FireMsg message;
        message.senderId = this->info->id;
        message.senderType = this->info->forceType;
        message.targetId = this->targetId;

        const Entity* targetEntity = env->QueryEntityById(targetId);
        if (!targetEntity) {
            logger_world << "[" << this->info->name << "] "
                         << "shoot at missing target (" << targetId << ")"
                         << " when Time : " << this->engine->GetCurrentTime()
                         << std::endl;
            return true;
        }

        std::any anyMessage = message;
        logger_world << "[" << this->info->name << "] "
                     << "shoot " << targetEntity->name
                     << " when Time : " << this->engine->GetCurrentTime()
                     << std::endl;
        this->AddOutputEvent("FireOut", anyMessage);
    } else {
        // miss: no event emitted
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
