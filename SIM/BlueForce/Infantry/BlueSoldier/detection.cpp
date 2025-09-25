#include "detection.hpp"

Detection::Detection(Engine* engine, int entityId, Entity* info)
    : AtomicModel(engine)
{
    this->entityId = entityId;
    this->info = info;

    this->AddState("WAIT");
    this->AddState("DETECT");

    this->SetCurState("WAIT");

    this->AddInputPort ("PositionIn");
    this->AddOutputPort ("SoldierRep");

    enemyIds.clear();
}

TIME_T Detection::detEquation() const{
    return 0.1f;
}

void Detection::RebuildEnemyPosList() {
    enemyIds.clear();
    Point curPos = this->info->position;
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
            if (e->side == this->info->side) continue;

            enemyIds.push_back(id);
        }
    }
}

bool Detection::ExtTransFn(const std::string& inPort, const std::any& anyMessage){
    if (inPort=="PositionIn") {
        PositionMsg message;
        if(!TryCastMessage(anyMessage,message,"")) return false;

        // 내 위치 갱신 : Maneuver가 끝나면 항상 update => 현재는 공유됨
    }
    this->RebuildEnemyPosList();
    if (!enemyIds.empty()) this->SetCurState("DETECT");
    else this->SetCurState("WAIT");
    return true;
}

bool Detection::OutputFn(){
    if(this->GetCurState()=="DETECT"){
        this->t_det = detEquation();

        SoldierRep message;
        message.entityId = this->entityId;
        message.enemyDetected = true;
        message.enemyIds = &this->enemyIds;
        std::any anyMessage = message;
        this->AddOutputEvent("detRes",anyMessage);

    }
    return true;
}

bool Detection::IntTransFn(){
    if (this->GetCurState() == "DETECT") {
        this->SetCurState("WAIT");
    }
    return true;
}

TIME_T Detection::TimeAdvanceFn(){
    if (this->GetCurState() == "WAIT")   return TIME_INF;
    if (this->GetCurState() == "DETECT") return t_det;
    return -1;   // 오류
}