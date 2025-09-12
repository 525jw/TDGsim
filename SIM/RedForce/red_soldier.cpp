#include "red_soldier.hpp"
#include <cmath>

RedSoldier::RedSoldier(Engine* engine, int entityId, Entity info)
    : AtomicModel(engine)
{
    this->entityId = entityId;
    this->info = info;

    this->AddState("WAIT");
    this->AddState("DETECT");
    this->AddState("FIRE");
    this->AddState("DEAD");

    this->SetCurState("WAIT");

    this->AddInputPort("FireIn");
    this->AddInputPort("PositionIn");
    this->AddOutputPort("FireOut");
    // Blue는 이동 후 Detection 수행하니 필요 없음
    // this->AddOutputPort("PositionOut") 
}

TIME_T RedSoldier::fireEquation(){
    return this->fireFreq;
}
// TIME_T RedSoldier::detEquation() const{
//     return 0.1f;
// }

void RedSoldier::RebuildEnemyPosList() {
    enemyIds.clear();
    Point curPos = this->info.position;
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

            const Entity* e = ENV_REF.QueryEntityById(id);
            if (!e) continue;
            if (e->team == this->info.team) continue;

            enemyIds.push_back(id);
        }
    }
}

bool RedSoldier::ExtTransFn(const std::string& inPort, const std::any& anyMessage) {
    if (inPort == "PositionIn") {
        PositionMsg message;
        if(!TryCastMessage(anyMessage,message,"")) return false;

        this->RebuildEnemyPosList();
        if (!enemyIds.empty()) this->SetCurState("DETECT");
        else this->SetCurState("WAIT");
    }else if(inPort == "FireIn"){
        FireMsg message;
        if(!TryCastMessage(anyMessage,message,"")) return false;

        // 본인에게 온 사격인지 탐색
        if( (message.senderType == ForceType::ARTILLERY && message.targetPoint == this->info.position) ||
            (message.senderType == ForceType::RIFLE && message.targetId == this->entityId) ){
                ENV->RequestKillEntity(this->entityId);
                this->SetCurState("DEAD");
        }
    }
    return true;
}

bool RedSoldier::OutputFn(){
    if (this->GetCurState() == "FIRE"){
        this->t_fire = this->fireEquation();

        ensureRng();
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        float roll = dist(rng);
        if (roll<=this->accuracy){
            FireMsg message;
            message.senderId = this->entityId;
            message.senderType = this->info.forceType;
            message.targetId = this->targetId;
            std::any anyMessage = message;
            this->AddOutputEvent("detRes",anyMessage);
        }
        else{
            // 빗나가면 메시지 전송 X, 추후 FireMsg.hit=false 방식 도입
        }
    }else if(this->GetCurState()=="DETECT"){
        // this->t_det = detEquation();
        ensureRng();
        std::uniform_int_distribution<size_t> dist(0, this->enemyIds.size() - 1);
        targetId = (this->enemyIds)[dist(rng)];
    }
    return true;
}

bool RedSoldier::IntTransFn(){
    if(this->GetCurState()=="DETECT"){
        this->SetCurState("FIRE");
    }
    return true;
}

TIME_T RedSoldier::TimeAdvanceFn(){
    if (this->GetCurState() == "WAIT") return TIME_INF;
    if (this->GetCurState() == "FIRE") return t_fire;

    return -1;
}