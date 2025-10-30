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

    std::string startStr = "DEFAULT";
    if (info->name.rfind(startStr, 0) == 0) {
        // name이 startStr로 시작
    }

}

TIME_T Fire::fireEquation(){
    return config::inf.fire_freq_rifle;
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
    if (this->GetCurState() == "FIRE") {
        ensureRng();
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        float roll = dist(rng);

        FireMsg message;
        message.senderId = this->info->id;
        message.senderType = this->info->forceType;

        const Entity* targetEntity = env->QueryEntityById(targetId);
        const bool targetAlive = targetEntity != nullptr;
        const bool hit = targetAlive && roll <= config::inf.fire_accuracy_rifle;

        if (hit) {
            message.targetId = this->targetId;
            message.targetPoint.push_back(Point{-1, -1});
            std::any anyMessage = message;
            LogSimulation(this->engine->GetCurrentTime(),this->GetName(),"FIRE","shoot at ",targetEntity->name);
            this->AddOutputEvent("FireOut", anyMessage);
        } else {
            message.targetId = -1;
            message.targetPoint.push_back(Point{-1, -1});
            std::any anyMessage = message;
            if (targetAlive) {
                LogSimulation(this->engine->GetCurrentTime(),this->GetName(),"FIRE","missed ",targetEntity->name);
            } else {
                LogSimulation(this->engine->GetCurrentTime(),this->GetName(),"FIRE","target already dead");
            }
            this->AddOutputEvent("FireOut", anyMessage);
        }

        return true;
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
