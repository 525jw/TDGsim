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
float Artillery::fireEquation(){
    return config::art.fire_freq_rps;
}
bool Artillery::ExtTransFn(const std::string& inPort, const std::any& anyMessage) {
    if (inPort == "CompanyOrd") {
        CompanyOrd message;
        if(!TryCastMessage(anyMessage,message,"")) return false;
        auto it = message.orders.find(this->info.id);
        if (it == message.orders.end()) {
            return true;
        }

        const auto& orderList = it->second; // std::vector<Order>
        for (const Order& ord : orderList) {
            if (ord.task == TaskType::BOMBARD) {
                this->targetPos = ord.to;
                this->t_fire=fireEquation();
                this->SetCurState("FIRE");
                break;
            }
        }
    }
    // else if (inPort == "PlatoonOrd") {
    //     PlatoonOrd message;
    //     if(!TryCastMessage(anyMessage,message,"")) return false;
    //     // 본인에게 온 명령인지 탐색
    //     auto it = message.orders.find(this->info.id);
    //     if (it == message.orders.end()) {
    //         return true;
    //     }
    //     const Order& ord = it->second;

    //     const auto& orderList = it->second; // std::vector<Order>

    //     if(ord.task == TaskType::BOMBARD){
    //         this->targetPos = ord.to;
    //         this->SetCurState("Fire");
    //     }
    // }
    return true;
}


bool Artillery::OutputFn(){
    if (this->GetCurState() == "FIRE") {
        this->curAmmo--;
        ensureRng();
        FireMsg message;
        message.senderId = this->info.id;
        message.senderType = this->info.forceType;

        message.targetPoint.clear();
        message.explosive_pkill = config::art.phit_he;
        constexpr float twoPi = 6.28318530717958647692f;
        std::uniform_real_distribution<float> angleDist(0.0f, twoPi);
        std::uniform_real_distribution<float> unitDist(0.0f, 1.0f);

        Point impactPoint = this->targetPos;
        const float rangeError = config::art.range_error;
        if (rangeError > 0.0f) {
            float angle = angleDist(rng);
            float radius = std::sqrt(unitDist(rng)) * rangeError;
            impactPoint.x = static_cast<int>(std::round(this->targetPos.x + std::cos(angle) * radius));
            impactPoint.y = static_cast<int>(std::round(this->targetPos.y + std::sin(angle) * radius));
        }

        if (EnvReady()) {
            int maxX = std::max(0, env->GetWidth() - 1);
            int maxY = std::max(0, env->GetHeight() - 1);
            impactPoint.x = std::clamp(impactPoint.x, 0, maxX);
            impactPoint.y = std::clamp(impactPoint.y, 0, maxY);
        }

        const float explosiveRange = config::art.explosive_range;
        const int radiusLimit = static_cast<int>(std::ceil(explosiveRange));

        for (int dx = -radiusLimit; dx <= radiusLimit; ++dx) {
            for (int dy = -radiusLimit; dy <= radiusLimit; ++dy) {
                Point candidate{impactPoint.x + dx, impactPoint.y + dy};
                if (EnvReady() && !env->InBounds(candidate)) continue;
                float distance = std::sqrt(static_cast<float>(dx * dx + dy * dy));
                if (distance > explosiveRange) continue;
                if (unitDist(rng) <= config::art.phit_he) {
                    if (std::find(message.targetPoint.begin(), message.targetPoint.end(), candidate) == message.targetPoint.end()) {
                        message.targetPoint.push_back(candidate);
                    }
                }
            }
        }

        std::ostringstream oss;
        oss << "=(" << impactPoint.x << "," << impactPoint.y << ") targets: ";
        for (size_t i = 0; i < message.targetPoint.size(); ++i) {
            const auto& p = message.targetPoint[i];
            oss << "(" << p.x << "," << p.y << ")";
            if (i + 1 < message.targetPoint.size()) oss << ", ";
        }
        LogSimulation(this->engine->GetCurrentTime(),this->GetName(),"FIRE",oss.str());



        std::any anyMessage = message;
        this->AddOutputEvent("FireOut", anyMessage);
        return true;
    }
    return true;
}

bool Artillery::IntTransFn(){
    if(this->GetCurState()=="FIRE" && this->curAmmo <= 0)
        this->SetCurState("WAIT");
    return true;
}

float Artillery::TimeAdvanceFn(){
    if (this->GetCurState() == "WAIT") return TIME_INF;
    if (this->GetCurState() == "FIRE") return t_fire;

    return -1;
}
