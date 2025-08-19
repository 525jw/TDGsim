#include "fire.hpp"
#include <cmath>

Fire::Fire(Engine* engine, int entityId)
    : AtomicModel(engine)
{
    this->entityId = entityId;

    this->AddState("WAIT");
    this->AddState("FIRE");

    this->SetCurState("WAIT");

    this->AddInputPort("DetRes"); // = SoldierRep
    this->AddOutputPort("FireRes");
}

TIME_T Fire::fireEquation(){
    return this->reloadTime;
}

bool Fire::ExtTransFn(const std::string& inPort, const std::any& anyMessage) {
    if (inPort == "DetRes" && this->GetCurState()=="WAIT") {
        SoldierRep message;
        if(!TryCastMessage(anyMessage,message,"")) return false;

        if(message.enemyDetected && !message.enemyIds){
            if(!rngInit){
                unsigned int envSeed =  0xA341316Cu;
                if(EnvGlobal::Ready()) envSeed = ENV_REF.GetSeed();

                std::uint32_t mix = envSeed
                    ^ (static_cast<std::uint32_t>(entityId) + 0x9e3779b9u + (envSeed<<6) + (envSeed>>2));
                rng.seed(mix);
                rngInit = true;
            }

            std::uniform_int_distribution<std::size_t> dist(0, message.enemyIds->size()-1);
            this->targetId = (*message.enemyIds)[dist(rng)];

            this->SetCurState("FIRE");
    
        }else{
            this->SetCurState("WAIT");
        }
    }
    return true;
}

bool Fire::IntTransFn(){
    if (this->GetCurState() == "FIRE")
        this->SetCurState("WAIT");
    return true;
}

bool Fire::OutputFn(){
    if (this->GetCurState() == "FIRE"){
        FireRes message;
        message.senderId = this->entityId;

        bool hit = false;
        if (this->targetId >= 0) {
            if (!rngInit) {
                unsigned int envSeed = 0xA341316Cu;
                if (EnvGlobal::Ready()) envSeed = ENV_REF.GetSeed();
                std::uint32_t mix = envSeed
                    ^ (static_cast<std::uint32_t>(entityId) + 0x9e3779b9u + (envSeed<<6) + (envSeed>>2));
                rng.seed(mix);
                rngInit = true;
            }

            // accuracy 확률로 명중 판정
            std::uniform_real_distribution<float> dist(0.0f, 1.0f);
            hit = (dist(rng) < this->accuracy);
        }
        message.hit = hit;
        
        std::any anyMessage = message;
        this->AddOutputEvent("detRes",anyMessage);
    }
    return true;
}

TIME_T Fire::TimeAdvanceFn(){
    if (this->GetCurState() == "WAIT") return TIME_INF;
    if (this->GetCurState() == "FIRE") return t_fire;

    return -1;
}