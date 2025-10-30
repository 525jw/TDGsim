#include "environment.hpp"
Environment* env = nullptr;
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

    assert(env == nullptr && "Only one Environment instance expected");
    env = this;
}

Environment::~Environment() {
    if (env == this) env = nullptr;
}
bool Environment::ExtTransFn(const std::string& inPort, const std::any& anyMessage) {
    if(inPort == "Start" && this->GetCurState()=="IDLE"){ // 초기화
        Start message;
        if(!TryCastMessage(anyMessage,message,"")) return false;
        const Scenario& s = *message.scen;

        // 기본필드 등록
        this->seed   = s.seed;
        this->width  = s.width;
        this->height = s.height;

        // 지형정보 등록
        terrainMap.assign(height, std::vector<TerrainType>(width, TerrainType::PLAIN));
        for (const auto& r : s.terrainRects) {
            int x1 = r.x1, y1 = r.y1, x2 = r.x2, y2 = r.y2;

            if (x1 > x2) std::swap(x1, x2);
            if (y1 > y2) std::swap(y1, y2);
            x1 = std::max(0, x1); y1 = std::max(0, y1);
            x2 = std::min(width  - 1, x2);
            y2 = std::min(height - 1, y2);

            for (int y = y1; y <= y2; ++y) {
                for (int x = x1; x <= x2; ++x) {
                    terrainMap[y][x] = r.kind; // TerrainRect.kind == TerrainType
                }
            }
        }

        // 엔티티정보 등록
        entities.clear();
        for (const auto& src : s.entities) {
            Entity e{};
            e.name      = src.name;
            e.side      = src.side;
            e.forceType = src.forceType;
            e.position  = src.position;

            // 맵 범위 보정
            if (e.position.x < 0) e.position.x = 0;
            if (e.position.y < 0) e.position.y = 0;
            if (e.position.x >= width)  e.position.x = width  - 1;
            if (e.position.y >= height) e.position.y = height - 1;

            const int assignedId = RegisterEntityIdByName(e.name);
            e.id = assignedId;
            entities[assignedId] = std::move(e);
        }
        LogSimulation(this->engine->GetCurrentTime(),this->GetName(),"ENV_INIT","seed=",this->seed);
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
    if (this->GetCurState() == "IDLE") return TIME_INF;
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

    it->second.position = p;

    return EnvMoveResponse::Accepted;
}

EnvKillResponse Environment::RequestKillEntity(int id) {
    auto it = entities.find(id);
    if (it == entities.end()) return EnvKillResponse::NotFound;

    Point p = QueryEntityPosById(id);
    nameToId.erase(entities[id].name);
    entities.erase(it);
    return EnvKillResponse::Accepted;
}

int Environment::RegisterEntityIdByName(const std::string& name){
    auto it = nameToId.find(name);
    if (it != nameToId.end()) return it->second;

    int id = nextId++;
    nameToId[name] = id;
    return id;
}
