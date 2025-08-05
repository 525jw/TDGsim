#include "detection.hpp"

Detection::Detection(Engine* engine, int objectID)
    : AtomicModel(engine)
{
    this->objectID = objectID;

    this->AddState("WAIT");
    this->AddState("DETECT");

    this->SetCurState("WAIT");

    this->AddInputPort ("envInfo");
    this->AddInputPort ("deadInfo");
    this->AddOutputPort("detRes");

    enemyPos.clear();
}

/* 맨해튼 거리 기반 탐지*/
void Detection::RebuildEnemyDistance(const EnvInfo& msg) {
    enemyPos.clear();
    if (curPos.first < 0 || curPos.second < 0) return;

    int cx = this->curPos.first;
    int cy = this->curPos.second;

    // NOTE: 팀 추정 로직 (임시)
    Team myTeam;
    bool foundSelf = false;

    for (const auto& [id, pos] : msg.redTeam) {
        if (pos == curPos) {
            myTeam = Team::RED;
            foundSelf = true;
            break;
        }
    }
    if (!foundSelf) {
        for (const auto& [id, pos] : msg.blueTeam) {
            if (pos == curPos) {
                myTeam = Team::BLUE;
                foundSelf = true;
                break;
            }
        }
    }
    if (!foundSelf) return; // 내 위치 못 찾으면 탐지 불가

    const int r = this->vision;
    const auto& enemyMap = (myTeam == Team::RED) ? msg.blueTeam : msg.redTeam;

    for (const auto& [eid, pos] : enemyMap) {
        int dx = std::abs(pos.first - cx);
        int dy = std::abs(pos.second - cy);
        int manhattan = dx + dy;

        if (manhattan <= r) {
            this->enemyPos[eid] = pos;
        }
    }
}

TIME_T Detection::detEquation() const{
    return 0.1f;
}

bool Detection::ExtTransFn(const std::string& inPort, const std::any& anyMessage){
    if (inPort=="envInfo") {
        EnvInfo message;
        if(!TryCastMessage(anyMessage,message,"")) return false;

        this->RebuildEnemyDistance(message);
    } else if (inPort=="mnvRes") {
        MnvRes message;
        if(!TryCastMessage(anyMessage,message,"")) return false;

        this->curPos=message.curPos;
    } else if (inPort=="deadInfo") {
        this->SetCurState("WAIT");
        enemyPos.clear();
        return true;
    }
    if (!enemyPos.empty()) this->SetCurState("DETECT");
    else this->SetCurState("WAIT");
    return true;
}


bool Detection::IntTransFn(){
    if (this->GetCurState() == "DETECT") {
        this->t_det = detEquation();
        // this->SetCurState("DETECT"); 이론적
    }
    return true;
}

bool Detection::OutputFn(){
    if(this->GetCurState()=="DETECT"){
        DetRes message;
        // if(EnemyIsVisible())
            message.enemyDetected = true;
        // else
            // message.enemyDetected = false;
        message.enemyPos = this->enemyPos;
        std::any anyMessage = message;
        this->AddOutputEvent("detRes",anyMessage);
    }
    return true;
}

TIME_T Detection::TimeAdvanceFn(){
    if (this->GetCurState() == "WAIT")   return TIME_INF;
    if (this->GetCurState() == "DETECT") return t_det;
    return -1;   // 오류
}