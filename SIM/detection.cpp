#include "detection.hpp"

Detection::Detection(int modelID, Engine* engine, int objectID)
    : AtomicModel(modelID, engine)
{
    this->objectID = objectID;

    this->AddState("WAIT");
    this->AddState("DETECT");
    this->SetCurState("WAIT");

    this->AddInputPort ("env_info");
    this->AddInputPort ("dead_in");
    this->AddOutputPort("det_result");

    enemy_pos.clear();
}

/* 맨해튼 거리 기반 탐지*/
void Detection::RebuildEnemyDistance(const EnvInfo& msg){
    enemy_pos.clear();
    if (cur_pos.first < 0 || cur_pos.second < 0) return;

    int cx = this->cur_pos.first;
    int cy = this->cur_pos.second;

    //NOTE : 팀을 식별하기 위한 코드, 현재는 개별 Action단위의 AM에 위임되어 있으니 따로 변수가 만들어 지면 삭제할것
    //------------------------------
    Team my_team = Team::BLUE;
    bool foundSelf = false;

    for (const auto& [id,pos] : msg.red_pos) {
        if (pos.first == cx && pos.second == cy) {
            my_team   = Team::RED;
            foundSelf = true;
            break;
        }
    }
    if (!foundSelf) {
        for (const auto& [id,pos] : msg.blue_pos) {
            if (pos.first == cx && pos.second == cy) {
                my_team   = Team::BLUE;
                foundSelf = true;
                break;
            }
        }
    }
    if (!foundSelf) return; // 내 위치를 못 찾으면 탐지 불가
     //------------------------------


    int r = this->vision;
    auto& enemy_map = (my_team == Team::RED) ? msg.blue_pos : msg.red_pos;

    for (const auto& [eid, pos] : enemy_map) {
        int dx = std::abs(pos.first  - cx);
        int dy = std::abs(pos.second - cy);
        int manhattan = dx + dy;
        if (manhattan <= r) {
            this->enemy_pos[eid] = pos;
        }
    }
}

TIME_T Detection::detEquation() const{
    return 0.1f;
}

bool Detection::ExtTransFn(const std::string& inPort, const std::any& message){
    if (inPort=="env_info") {
        try {
            const EnvInfo& msg = std::any_cast<const EnvInfo&>(message);
            
            RebuildEnemyDistance(msg); // env info는 항상 dist_from_enemy 갱신

        } catch (const std::bad_any_cast&) {
            logger_system << "[ERROR] Invalid message! port name : env_info" << std::endl;
            return false;
        }
    }
    else if (inPort=="mnv_result") {
        try {
            const MnvResult& msg = std::any_cast<const MnvResult&>(message);

            this->cur_pos = msg.cur_pos; // mnv_result는 항상 cur_pos 갱신

        } catch (const std::bad_any_cast&) {
            logger_system << "[ERROR] Invalid message! port name : mnv_result" << std::endl;
            return false;
        }
    }
    else if (inPort=="dead_in") {
        SetCurState("WAIT");
        enemy_pos.clear();
        return true;
    }
    if (!enemy_pos.empty()) SetCurState("DETECT");
    else SetCurState("WAIT");
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
    DetResult msg_raw;
    msg_raw.enemy_pos = this->enemy_pos;
    if (this->GetCurState() == "DETECT") msg_raw.IsDetect = true;
    else msg_raw.IsDetect = false;
    std::any msg_final = msg_raw;
    this->AddOutputEvent("det_result", msg_final);
    return true;
}

TIME_T Detection::TimeAdvanceFn()
{
    if (this->GetCurState() == "WAIT")   return TIME_INF;
    if (this->GetCurState() == "DETECT") return t_det;
    return -1;   // 오류
}

void Detection::UpdateTime(const TIME_T currentTime)
{
    this->lastTime = currentTime;
    this->nextTime = currentTime + TimeAdvanceFn();
}
void Detection::ReceiveScheduleTime(const TIME_T currentTime)
{
    AtomicModel::ReceiveScheduleTime(currentTime);
}
void Detection::ReceiveEvent(Event& event, TIME_T currentTime)
{
    AtomicModel::ReceiveEvent(event, currentTime);
}
const TIME_T Detection::QueryNextTime() const
{
    return AtomicModel::QueryNextTime();
}