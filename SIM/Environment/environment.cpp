#include "environment.hpp"

Environment::Environment(Engine* engine)
    : AtomicModel(engine)
{
    this->AddState("IDLE");
    this->AddState("WAIT");
    // this->AddState("UPDATE");

    this->SetCurState("IDLE");

    this->AddInputPort("Start");

    // this->AddInputPort("BluePosition");
    // this->AddInputPort("RedPosition");

    assert(EnvGlobal::ptr == nullptr && "Only one Environment instance expected");
    EnvGlobal::ptr = this;
}

Environment::~Environment() {
    if (EnvGlobal::ptr == this) EnvGlobal::ptr = nullptr;
}
void Environment::TempMapGen(){
    // MAP SETTING HARD COD
    this->width  = 30;
    this->height = 18;
    terrainTypeMap.assign(height, std::vector<TerrainType>(width, TerrainType::PLAIN));
    auto paintRect = [&](int x0, int y0, int x1, int y1, TerrainType t) {
        if (x0 > x1) std::swap(x0,x1);
        if (y0 > y1) std::swap(y0,y1);
        x0 = std::max(0, x0); y0 = std::max(0, y0);
        x1 = std::min(width-1,  x1);
        y1 = std::min(height-1, y1);
        for (int y = y0; y <= y1; ++y)
            for (int x = x0; x <= x1; ++x)
                terrainTypeMap[y][x] = t;
    };
    paintRect(20, 3, 23, 5, TerrainType::RIVER);
    paintRect(10, 6, 13, 10, TerrainType::RIVER);
    paintRect(22, 11, 25, 14, TerrainType::RIVER);
    auto addEntity = [&](int id, Team team, ForceType ft, int x, int y, const std::string& name) {
        Entity e;
        e.team = team;
        e.forceType = ft;
        e.position = {x,y};
        e.name = name;
        entities[id] = std::move(e);
    };
    addEntity( 1, Team::BLUE, ForceType::RIFLE, 3, 14, "BLUE-PLT1-SOL1");
    addEntity( 2, Team::BLUE, ForceType::RIFLE, 5, 15, "BLUE-PLT1-SOL2");
    addEntity( 3, Team::BLUE, ForceType::RIFLE, 4, 13, "BLUE-PLT1-SOL3");
    addEntity( 5, Team::BLUE, ForceType::RIFLE, 2, 12, "BLUE-TNK");
    addEntity( 6, Team::BLUE, ForceType::RIFLE, 1, 17, "BLUE-ART");


    addEntity( 7, Team::RED,  ForceType::RIFLE, 24,  3, "RED-PLT1-SOL1");
    addEntity( 8, Team::RED,  ForceType::RIFLE, 23,  3, "RED-PLT1-SOL2");
    addEntity( 9, Team::RED,  ForceType::RIFLE, 24,  2, "RED-PLT1-SOL3");
    addEntity(12, Team::RED,  ForceType::RIFLE, 29, 10, "RED-TNK");
    addEntity(13, Team::RED,  ForceType::RIFLE, 28,  1, "RED-ART");

    entityIdMap.assign(height, std::vector<int>(width, -1));
    for (const auto& kv : entities) {
        int id = kv.first;
        const auto& ent = kv.second;
        if (ent.position.x >= 0 && ent.position.x < width &&
            ent.position.y >= 0 && ent.position.y < height)
        {
            entityIdMap[ent.position.y][ent.position.x] = id;
        }
    }
}
bool Environment::ExtTransFn(const std::string& inPort, const std::any& anyMessage) {
    if(inPort == "Start" && this->GetCurState()=="IDLE"){ // 초기화
        Start message;
        if(!TryCastMessage(anyMessage,message,"")) return false;
        
        this->seed=message.seed;
        // 맵 생성
        TempMapGen();
        
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

EnvMoveResponse Environment::RequestMoveEntity(int id,Point p){
    if (!InBounds(p)) return EnvMoveResponse::OutOfBounds;

    auto it = entities.find(id);
    if (it == entities.end()) return EnvMoveResponse::NotFound;

    if (GetTerrainAt(p) == TerrainType::RIVER) {
        return EnvMoveResponse::InvalidTerrain;
    }

    Point old = it->second.position;
    if (old.x == p.x && old.y == p.y) return EnvMoveResponse::NoOp;

    int& dstCell = entityIdMap[p.y][p.x];
    if (dstCell != -1 && dstCell != id) {
        return EnvMoveResponse::OccupiedOther;
    }

    if (InBounds(old) && entityIdMap[old.y][old.x] == id) {
        entityIdMap[old.y][old.x] = -1;
    }

    dstCell = id;
    it->second.position = p;

    return EnvMoveResponse::Accepted;
}

EnvKillResponse Environment::RequestKillEntity(int id) {
    auto it = entities.find(id);
    if (it == entities.end()) return EnvKillResponse::NotFound;

    Point p = QueryEntityPosById(id);
    entityIdMap[p.y][p.x] = -1;
    entities.erase(it);

    return EnvKillResponse::Accepted;
}