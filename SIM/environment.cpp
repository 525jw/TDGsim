#include "environment.hpp"

Environment::Environment(Engine* engine)
    : AtomicModel(engine)
{
    this->AddState("IDLE");
    this->AddState("WAIT");
    this->AddState("UPDATE");

    this->SetCurState("IDLE");

    this->AddInputPort("ScenInfo");

    this->AddInputPort("mnvRes");
    this->AddInputPort("deadInfo"); // TODO피해결과로 수정

    this->AddOutputPort("EnvInfo");

    assert(EnvGlobal::ptr == nullptr && "Only one Environment instance expected");
    EnvGlobal::ptr = this;
}

Environment::~Environment() {
    if (EnvGlobal::ptr == this) EnvGlobal::ptr = nullptr;
}

bool Environment::ExtTransFn(const std::string& inPort, const std::any& anyMessage) {
    if(inPort == "mnvRes" && this->GetCurState() == "WAIT"){ // 기동결과 : entity 위치 갱신
        MnvRes message;
        if(!TryCastMessage(anyMessage,message,"")) return false;

        auto it = entities.find(message.entityID);
        if (it == entities.end())
            return false;
        int newX = message.curPos.first;
        int newY = message.curPos.second;
        if (!InBounds(newX, newY)) // 맵 범위 체크
            return false;
        if (entityMap[newY][newX] != -1) // 단일 점유 체크 - location이 key인 table 필요
            return false;
        // 기존 좌표 비우기
        int oldX = it->second.x;
        int oldY = it->second.y;
        if (InBounds(oldX, oldY) && entityMap[oldY][oldX] == message.entityID)
            entityMap[oldY][oldX] = -1;
        // 새 위치 갱신
        it->second.x = newX;
        it->second.y = newY;
        entityMap[newY][newX] = message.entityID;

        this->SetCurState("UPDATE");
    }else if(inPort == "deadInfo" && this->GetCurState()=="WAIT"){ // 사격결과 : entity 삭제
        DeadInfo message;
        if(!TryCastMessage(anyMessage,message,"")) return false;

        int id = message.entityID;
        auto it = entities.find(id);
        if (it == entities.end())
            return false;
        int x = it->second.x;
        int y = it->second.y;
        if (InBounds(x, y) && entityMap[y][x] == id)
            entityMap[y][x] = -1;
        entities.erase(it);

        this->SetCurState("UPDATE");
    }else if(inPort == "ScenInfo" && this->GetCurState()=="IDLE"){ // 초기화
        ScenInfo message;
        if(!TryCastMessage(anyMessage,message,"")) return false;
        
        this->seed=message.seed;
        this->InitScenario_BlueRiver(); // NOTE : 현재는 시나리오 생성 Environment 내에서 담당

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
        EnvInfo message;
        std::any anyMessage = message;
        this->AddOutputEvent("EnvInfo",anyMessage);
    }
    return true;
}

TIME_T Environment::TimeAdvanceFn() {
    if (this->GetCurState() == "WAIT") return TIME_INF;
    if (this->GetCurState() == "UPDATE") return 0.0f;
    return -1;
}






// 기본 맵 생성
void Environment::InitMap(int w, int h) {
    terrainMap.assign(h, std::vector<TerrainType>(w, TerrainType::PLAIN));
    entityMap.assign(h, std::vector<int>(w, -1));
}

// 경계 체크
bool Environment::InBounds(int x,int y) const noexcept {
    return (y >= 0 && y < (int)terrainMap.size() &&
            x >= 0 && !terrainMap.empty() && x < (int)terrainMap[0].size());
}

// 셀 칠하기
void Environment::PaintRect(int x0,int y0,int x1,int y1, TerrainType t) {
    if (x0 > x1) std::swap(x0,x1);
    if (y0 > y1) std::swap(y0,y1);
    x0 = std::max(0, x0); y0 = std::max(0, y0);
    x1 = std::min((int)terrainMap[0].size()-1, x1);
    y1 = std::min((int)terrainMap.size()-1,  y1);
    for (int y=y0; y<=y1; ++y)
        for (int x=x0; x<=x1; ++x)
            terrainMap[y][x] = t;
}

// 타원(언덕/숲 등 덩어리) 칠하기
void Environment::PaintEllipse(int cx,int cy,int rx,int ry, TerrainType t) {
    int x0 = std::max(0, cx - rx), x1 = std::min((int)terrainMap[0].size()-1, cx + rx);
    int y0 = std::max(0, cy - ry), y1 = std::min((int)terrainMap.size()-1,  cy + ry);
    for (int y=y0; y<=y1; ++y) {
        for (int x=x0; x<=x1; ++x) {
            double dx = double(x - cx) / std::max(1, rx);
            double dy = double(y - cy) / std::max(1, ry);
            if (dx*dx + dy*dy <= 1.0) terrainMap[y][x] = t;
        }
    }
}

// 수직 도로(폭 width) 칠하기
void Environment::PaintRoadVertical(int x, int y0, int y1, int width=8) {
    int half = std::max(1, width/2);
    for (int y = std::max(0,y0); y <= std::min(y1, (int)terrainMap.size()-1); ++y) {
        for (int dx = -half; dx <= half; ++dx) {
            int xx = x + dx;
            if (InBounds(xx, y)) terrainMap[y][xx] = TerrainType::ROAD;
        }
    }
}

// 강(수평 밴드) 칠하기
void Environment::PaintRiverBand(int yCenter, int thickness=60) {
    int half = std::max(1, thickness/2);
    int y0 = std::max(0, yCenter - half);
    int y1 = std::min((int)terrainMap.size()-1, yCenter + half);
    for (int y=y0; y<=y1; ++y)
        for (int x=0; x<(int)terrainMap[0].size(); ++x)
            terrainMap[y][x] = TerrainType::RIVER;
}

// 다리/도하로: 강 위를 도로로 다시 덮기
void Environment::PaintBridge(int x, int y0, int y1, int width=8) {
    int half = std::max(1, width/2);
    for (int y = std::max(0,y0); y <= std::min(y1, (int)terrainMap.size()-1); ++y)
        for (int dx=-half; dx<=half; ++dx) {
            int xx = x + dx;
            if (InBounds(xx, y)) terrainMap[y][xx] = TerrainType::ROAD;
        }
}

// 엔티티 스폰 (단일 점유)
bool Environment::SpawnEntity(int id, int x, int y, ForceType ft) {
    if (!InBounds(x,y) || entityMap[y][x] != -1 || entities.count(id)) return false;
    entities[id] = Entity{x,y,ft};
    entityMap[y][x] = id;
    return true;
}

// === 시나리오 구성 ===
// 1m 셀 기준: 1600 x 1400
void Environment::InitScenario_BlueRiver() {
    const int W = 1600, H = 1400; // 약 1.6km x 1.4km
    InitMap(W, H);

    // 1) 강: 그림 중앙 수평 (하천 폭 ~60m)
    int riverY = 700;
    PaintRiverBand(riverY, /*thickness=*/60);

    // 2) 도로: 좌측/우측 수직 간선 2개 (폭 ~8m)
    int roadL = 520;   // 좌측 도로 x
    int roadR = 1080;  // 우측 도로 x
    PaintRoadVertical(roadL, 0, H-1, 8);
    PaintRoadVertical(roadR, 0, H-1, 8);

    // 3) 교량: 두 도로가 강을 건너는 구간(강 구간을 ROAD로 덮음)
    PaintBridge(roadL, riverY-40, riverY+40, 8);
    PaintBridge(roadR, riverY-40, riverY+40, 8);

    // 4) 북측 광범위한 구릉(큰 타원)
    //    그림 상단의 긴 타원 지형
    PaintEllipse(/*cx=*/800, /*cy=*/1150, /*rx=*/450, /*ry=*/120, TerrainType::HILL);

    // 5) 중부 숲 (강 북쪽 좌측의 불규칙 패치 → 타원 근사)
    PaintEllipse(/*cx=*/600, /*cy=*/900, /*rx=*/120, /*ry=*/90, TerrainType::FOREST);

    // 6) 강 남쪽 중앙의 독립 구릉
    PaintEllipse(/*cx=*/800, /*cy=*/450, /*rx=*/120, /*ry=*/170, TerrainType::HILL);

    // 7) 동측 중부 숲 패치 (그림 우측 원 내 녹지 근사)
    PaintEllipse(/*cx=*/1020, /*cy=*/600, /*rx=*/110, /*ry=*/80, TerrainType::FOREST);

    // 8) 기타 평지(기본값 PLAIN)라 생략

    // === 병력 배치(예시) ===
    // BLUE INF: 강 남쪽 좌우 집결지 부근
    //   - 좌측 집결지(roadL 남쪽) 주위
    SpawnEntity(1001, roadL - 40, 620, ForceType::BLUE_INF);
    SpawnEntity(1002, roadL - 30, 640, ForceType::BLUE_INF);
    SpawnEntity(1003, roadL - 20, 660, ForceType::BLUE_INF);
    SpawnEntity(1004, roadL - 10, 640, ForceType::BLUE_INF);
    SpawnEntity(1005, roadL - 25, 620, ForceType::BLUE_INF);

    //   - 우측 집결지(roadR 남쪽) 주위
    SpawnEntity(1101, roadR + 20, 620, ForceType::BLUE_INF);
    SpawnEntity(1102, roadR + 30, 640, ForceType::BLUE_INF);
    SpawnEntity(1103, roadR + 40, 660, ForceType::BLUE_INF);
    SpawnEntity(1104, roadR + 25, 640, ForceType::BLUE_INF);
    SpawnEntity(1105, roadR + 35, 620, ForceType::BLUE_INF);

    // RED INF: 강 북쪽의 두 교두보/도로 감시, 그리고 북측 구릉 방어선
    //   - 우측 도로 북안 감시
    SpawnEntity(2001, roadR + 15, riverY + 80, ForceType::RED_INF);
    SpawnEntity(2002, roadR - 15, riverY + 80, ForceType::RED_INF);

    //   - 좌측 도로 북안 감시
    SpawnEntity(2101, roadL + 15, riverY + 80, ForceType::RED_INF);
    SpawnEntity(2102, roadL - 15, riverY + 80, ForceType::RED_INF);

    //   - 북측 구릉(큰 타원) 내부 방어 배치
    for (int i=0; i<8; ++i) {
        int x = 800 - 300 + i*80;    // 넓게 퍼짐
        int y = 1130 + ((i%2)?30:-30);
        if (InBounds(x,y) && entityMap[y][x] == -1)
            SpawnEntity(3000+i, x, y, ForceType::RED_INF);
    }
}