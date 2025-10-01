#include "blue_cgf.hpp"

BlueCgf::BlueCgf(Engine* engine, int entityId, Entity info)
    : AtomicModel(engine)
{
    this->entityId = entityId;
    this->info = info;

    this->AddState("IDLE");
    this->AddState("DETECT");
    this->AddState("FIRE");
    this->AddState("DEAD");

    this->SetCurState("IDLE");

    this->AddInputPort("FireIn");
    this->AddInputPort("Start");
    this->AddOutputPort("FireOut");
    this->LogMyBirth();
}

TIME_T BlueCgf::fireEquation(){
    return this->fireFreq;
}
// TIME_T BlueCgf::detEquation() const{
//     return 0.1f;
// }

void BlueCgf::RebuildEnemyPosList() {
    enemyIds.clear();
    Point curPos = this->info.position;
    if (!EnvReady() || !env->InBounds(curPos)) return;
    const int H = env->GetHeight();
    const int W = env->GetWidth();
    const int vx = curPos.x;
    const int vy = curPos.y;

    const int y0 = std::max(0,     vy - vision);
    const int y1 = std::min(H - 1, vy + vision);
    const int x0 = std::max(0,     vx - vision);
    const int x1 = std::min(W - 1, vx + vision);

    const int r2 = vision * vision; // 유클리드 거리 제곱 비교

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
                if (id == this->entityId) continue;

                const Entity* e = env->QueryEntityById(id);
                if (!e) continue;
                if (e->side == this->info.side) continue;

                if (seen.insert(id).second) {
                    enemyIds.push_back(id);
                }
            }
        }
    }
}

bool BlueCgf::ExtTransFn(const std::string& inPort, const std::any& anyMessage) {
    if (inPort == "Start" && this->GetCurState() == "IDLE") {
        Start message;
        if(!TryCastMessage(anyMessage,message,"")) return false;

        this->SetCurState("DETECT");
    }else if(inPort == "FireIn"){
        FireMsg message;
        if(!TryCastMessage(anyMessage,message,"")) return false;
        logger_world << "["<<this->info.name<<"] "<<"got message, targetId : "<<env->QueryEntityById(message.targetId)->name<<" and my Id is : "<<env->QueryEntityById(this->entityId)->name<<std::endl;
        // 본인에게 온 사격인지 탐색
        if( (message.senderType == ForceType::ARTILLERY && message.targetPoint == this->info.position) ||
            (message.senderType == ForceType::RIFLE && message.targetId == this->entityId) ){
                env->RequestKillEntity(this->entityId);
                this->SetCurState("DEAD");
                logger_world << "["<<this->info.name<<"] "<< "is dead"<<" when Time : "<<this->engine->GetCurrentTime()<<std::endl;
        }
    }
    return true;
}

bool BlueCgf::OutputFn(){
    if (this->GetCurState() == "FIRE"){
        ensureRng();
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        float roll = dist(rng);
        if (roll<=this->accuracy){
            // 맞았다면 event enque
            FireMsg message;
            message.senderId = this->entityId;
            message.senderType = this->info.forceType;
            message.targetId = this->targetId;
            std::any anyMessage = message;
            logger_world << "["<<this->info.name<<"] "<< "shoots " <<env->QueryEntityById(this->targetId)->name<<" when Time : "<<this->engine->GetCurrentTime()<<std::endl;
            this->AddOutputEvent("FireOut",anyMessage);
        }
        else{
            // 빗나가면 메시지 전송 X, 추후 FireMsg.hit=false 방식 도입
        }
    }else if(this->GetCurState()=="DETECT"){
        this->RebuildEnemyPosList();
        if (!enemyIds.empty()) {
            logger_world << "["<<this->info.name<<"] "<< "detected something"<<" when Time : "<<this->engine->GetCurrentTime()<<std::endl;
            ensureRng();
            std::uniform_int_distribution<size_t> dist(0, this->enemyIds.size() - 1);
            targetId = (this->enemyIds)[dist(rng)];
        }
        // this->t_det = detEquation();
    }
    return true;
}

bool BlueCgf::IntTransFn(){
    if(this->GetCurState()=="DETECT"){
        this->t_fire = this->fireEquation();
        this->SetCurState("FIRE");
    }
    return true;
}

TIME_T BlueCgf::TimeAdvanceFn(){
    if (this->GetCurState() == "IDLE") return TIME_INF;
    if (this->GetCurState() == "DETECT") return 0.0f;
    if (this->GetCurState() == "FIRE") return this->t_fire;
    if (this->GetCurState() == "DEAD") return TIME_INF;

    return -1;
}