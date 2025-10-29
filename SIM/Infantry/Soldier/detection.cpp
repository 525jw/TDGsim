#include "detection.hpp"

Detection::Detection(Engine* engine, Entity* info)
    : AtomicModel(engine)
{
    this->info = info;

    this->AddState("WAIT");
    this->AddState("DETECT");

    this->SetCurState("WAIT");

    this->AddInputPort("MyPosition");
    // this->AddInputPort("EnemyPosition");
    this->AddOutputPort("SoldierRep");

    enemyIds.clear();


    this->UpdateTime(0.0f);
}

TIME_T Detection::detEquation() const{
    return 0.0f;
}

void Detection::RebuildEnemyPosList() {
    enemyIds.clear();
    Point curPos = this->info->position;
    if (!EnvReady() || !env->InBounds(curPos)) return;
    const int H = env->GetHeight();
    const int W = env->GetWidth();
    const int vx = curPos.x;
    const int vy = curPos.y;

    const int y0 = std::max(0,     vy - vision);
    const int y1 = std::min(H - 1, vy + vision);
    const int x0 = std::max(0,     vx - vision);
    const int x1 = std::min(W - 1, vx + vision);

    const int r2 = vision * vision;

    std::unordered_set<int> seen;

    for (int y = y0; y <= y1; ++y) {
        for (int x = x0; x <= x1; ++x) {
            const int dx = x - vx;
            const int dy = y - vy;
            const int d2 = dx*dx + dy*dy;
            if (d2 > r2) continue;

            const auto ids = env->QueryEntityIdsAt(Point{x, y});
            if (ids.empty()) continue;

            for (int id : ids) {
                if (id == this->info->id) continue;

                const Entity* e = env->QueryEntityById(id);
                if (!e) continue;
                if (e->side == this->info->side) continue;

                if (seen.insert(id).second) {
                    enemyIds.push_back(id);
                }
            }
        }
    }
}
bool Detection::ExtTransFn(const std::string& inPort, const std::any& anyMessage){
    if (inPort=="MyPosition") {
        PositionMsg message;
        if(!TryCastMessage(anyMessage,message,"")) return false;
        this->SetCurState("DETECT");
        this->t_det = this->detEquation();
    }
    
    return true;
}

bool Detection::OutputFn(){
    if(this->GetCurState()!="DETECT" && this->GetCurState()!="WAIT"){
        return true;
    }

    // LogSimulation(this->engine->GetCurrentTime(),this->GetName(),"DETECT","looks around");
    this->RebuildEnemyPosList();
    
    SoldierRep message;
    if (!enemyIds.empty()) {
        message.enemyDetected = true;
        // LogSimulation(this->engine->GetCurrentTime(),this->GetName(),"DETECT","detected somthing");
    }else{
        message.enemyDetected = false;
    }
    message.entityId = this->info->id;
    message.enemyIds = &this->enemyIds;
    std::any anyMessage = message;
    this->AddOutputEvent("SoldierRep",anyMessage);
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
    return -1;
}
