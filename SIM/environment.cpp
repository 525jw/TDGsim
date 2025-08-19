#include "environment.hpp"

Environment::Environment(Engine* engine)
    : AtomicModel(engine)
{
    this->AddState("IDLE");
    this->AddState("WAIT");
    this->AddState("UPDATE");

    this->SetCurState("IDLE");

    this->AddInputPort("ScenInfo");

    this->AddInputPort("MnvRes");
    this->AddInputPort("DmgRes"); // TODO피해결과로 수정

    this->AddOutputPort("EnvInfo");

    assert(EnvGlobal::ptr == nullptr && "Only one Environment instance expected");
    EnvGlobal::ptr = this;
}

Environment::~Environment() {
    if (EnvGlobal::ptr == this) EnvGlobal::ptr = nullptr;
}

bool Environment::ExtTransFn(const std::string& inPort, const std::any& anyMessage) {
    if(inPort == "MnvRes" && this->GetCurState() == "WAIT"){ // 기동결과 : entity 위치 갱신
        MnvRes message;
        if(!TryCastMessage(anyMessage,message,"")) return false;

        const Entity* ent = nullptr;
        if (!SearchEntityById(message.senderId, ent)) return false;
        Point curPos = ent->location;
        Point newPos = message.curPos;

        if (entityIdMap[newPos.y][newPos.x] != -1) // 단일 점유 체크
            return false;
        if (!InBounds(newPos)) // 맵 범위 체크
            return false;
        // 기존 좌표 비우기
        if (entityIdMap[curPos.y][curPos.x] == message.senderId)
            entityIdMap[curPos.y][curPos.x] = -1;
        // 새 위치 갱신
        entityIdMap[newPos.y][newPos.x] = message.senderId;

        this->SetCurState("UPDATE");
    }else if(inPort == "DmgRes" && this->GetCurState()=="WAIT"){ // 사격결과 : entity 삭제
        DmgRes message;
        if(!TryCastMessage(anyMessage,message,"")) return false;

        const Entity* ent = nullptr;
        if (!SearchEntityById(message.senderId, ent)) return false;
        Point curPos = ent->location;

        if (InBounds(curPos) && entityIdMap[curPos.y][curPos.x] == message.senderId)
            entityIdMap[curPos.y][curPos.x] = -1;
        entities.erase(message.senderId);

        this->SetCurState("UPDATE");
    }else if(inPort == "ScenInfo" && this->GetCurState()=="IDLE"){ // 초기화
        ScenInfo message;
        if(!TryCastMessage(anyMessage,message,"")) return false;
        
        this->seed=message.seed;
        // 맵 생성
        
        this->SetCurState("WAIT");
    }
    return true;
}

bool Environment::IntTransFn() {
    if(this->GetCurState()=="UPDATE"){
        this->SetCurState("WAIT");
    }
    return true;
}

bool Environment::OutputFn() {
    if(this->GetCurState()=="UPDATE"){
        
    }
    return true;
}

TIME_T Environment::TimeAdvanceFn() {
    if (this->GetCurState() == "WAIT") return TIME_INF;
    if (this->GetCurState() == "UPDATE") return 0.0f;
    return -1;
}


bool Environment::SpawnEntity(int id, Point pos, Team team, ForceType ft, std::string_view name) {
    if (!InBounds(pos)) return false;
    if (entityIdMap[pos.y][pos.x] != -1) return false;

    Entity e{team, pos, ft, std::string(name)};
    entities[id] = std::move(e);
    entityIdMap[pos.y][pos.x] = id;
    return true;
}

int Environment::RegisterEntity(Point pos, Team team, ForceType ft, std::string_view name) {
    if (!InBounds(pos)) return -1;
    if (entityIdMap[pos.y][pos.x] != -1) return -1;

    const int id = nextEntityId++;
    const bool ok = SpawnEntity(id, pos, team, ft, name);
    return ok ? id : -1;
}