#include "red_soldier.hpp"
#include <cmath>

RedSoldier::RedSoldier(Engine* engine, int entityId, std::string name)
    : AtomicModel(engine)
{
    this->entityId = entityId;
    this->name = name;

    this->AddState("WAIT");
    this->AddState("FIRE");

    this->SetCurState("WAIT");

    this->AddInputPort("start");
    this->AddInputPort("BlueFire");
    this->AddInputPort("BluePosition");
    this->AddOutputPort("RedFire");
}

TIME_T RedSoldier::fireEquation(){
    return this->reloadTime;
}

void RedSoldier::RebuildEnemyPosList() {
    enemyIds.clear();
    if (!EnvGlobal::Ready() || !ENV_REF.InBounds(curPos)) return;
    const int H = ENV_REF.GetHeight();
    const int W = ENV_REF.GetWidth();
    const int vx = curPos.x;
    const int vy = curPos.y;

    const int y0 = std::max(0,     vy - vision);
    const int y1 = std::min(H - 1, vy + vision);
    const int x0 = std::max(0,     vx - vision);
    const int x1 = std::min(W - 1, vx + vision);

    const int r2 = vision * vision; // 유클리드 거리 제곱 비교

    for (int y = y0; y <= y1; ++y) {
        for (int x = x0; x <= x1; ++x) {
            const int dx = x - vx;
            const int dy = y - vy;
            const int d2 = dx*dx + dy*dy;
            if (d2 > r2) continue;

            int id = ENV_REF.GetEntityIdAt(Point{x,y});
            if (id < 0 || id == this->entityId) continue;

            const Entity* e = ENV_REF.SearchEntityById(id);
            if (!e) continue;
            if (e->team == team) continue;

            enemyIds.push_back(id);
        }
    }
}

bool RedSoldier::ExtTransFn(const std::string& inPort, const std::any& anyMessage) {
    if (inPort == "BluePosition") {

        this->RebuildEnemyPosList();
        if (!enemyIds.empty()) this->SetCurState("FIRE");
        else this->SetCurState("WAIT");
        SoldierRep message;
    }
    return true;
}

bool RedSoldier::IntTransFn(){
    return true;
}

bool RedSoldier::OutputFn(){
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

TIME_T RedSoldier::TimeAdvanceFn(){
    if (this->GetCurState() == "WAIT") return TIME_INF;
    if (this->GetCurState() == "FIRE") return t_fire;

    return -1;
}