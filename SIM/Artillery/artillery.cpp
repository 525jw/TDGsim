#include "artillery.hpp"

Artillery::Artillery(Engine* engine, Entity info)
    : AtomicModel(engine)
{
    this->info=info;
    this->SetModelName(info.name);

    this->AddState("WAIT");
    this->AddState("FIRE"); // DECIDE and ORDER

    this->SetCurState("WAIT");

    this->AddInputPort("CompanyOrd");
    this->AddOutputPort("FireOut");
}
TIME_T Artillery::fireEquation(){
    return this->fireFreq;
}
bool Artillery::ExtTransFn(const std::string& inPort, const std::any& anyMessage) {
    if (inPort == "CompanyOrd") {
        PlatoonOrd message;
        if(!TryCastMessage(anyMessage,message,"")) return false;
        // 본인에게 온 명령인지 탐색
        auto it = message.orders.find(this->info.id);
        if (it == message.orders.end()) {
            return true;
        }
        const Order& ord = it->second;

        const auto& orderList = it->second; // std::vector<Order>

        if(ord.task == TaskType::BOMBARD){
            this->targetPos = ord.to;
            this->SetCurState("Fire");
        }
    }
    return true;
}


bool Artillery::OutputFn(){
    if (this->GetCurState() == "FIRE") {
        this->ammo--;
        ensureRng();
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        float roll = dist(rng);

        FireMsg message;
        message.senderId = this->info.id;
        message.senderType = this->info.forceType;

        // TODO: targetPos에서 range거리 이내로 fatality 수만큼 위치를 지정해서 output으
        std::any anyMessage = message;
        // LogSimulation(this->engine->GetCurrentTime(),this->GetName(),"FIRE","shoot");
        this->AddOutputEvent("FireOut", anyMessage);
        return true;
    }
    return true;
}

bool Artillery::IntTransFn(){
    if(this->GetCurState()=="FIRE" && this->ammo <= 0)
        this->SetCurState("WAIT");
    return true;
}

TIME_T Artillery::TimeAdvanceFn(){
    if (this->GetCurState() == "WAIT") return TIME_INF;
    if (this->GetCurState() == "FIRE") return t_fire;

    return -1;
}
