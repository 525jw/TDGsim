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

        message.targetPoint.clear();
        int targetCount = std::max(0, static_cast<int>(std::round(this->fatality)));
        message.targetPoint.reserve(static_cast<std::size_t>(targetCount));

        constexpr float twoPi = 6.28318530717958647692f;
        std::uniform_real_distribution<float> angleDist(0.0f, twoPi);
        std::uniform_real_distribution<float> unitDist(0.0f, 1.0f);

        for (int i = 0; i < targetCount; ++i) {
            float angle = angleDist(rng);
            float radius = std::sqrt(unitDist(rng)) * this->targetRange;
            int x = static_cast<int>(std::round(this->targetPos.x + std::cos(angle) * radius));
            int y = static_cast<int>(std::round(this->targetPos.y + std::sin(angle) * radius));
            message.targetPoint.push_back({x, y});
        }

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
