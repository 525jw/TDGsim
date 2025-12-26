#pragma once
#include <utility>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <optional>
#include <algorithm>
#include <cassert>
#include <string>
#include <random>
#include <fstream>
#include <limits>
#include <ctime>
#include <cmath>
#include <cctype>
#include <cstdint>
#include <functional>
#include <chrono>
#include "utils/json.hpp"
#include "utils/geometry.hpp"
using json = nlohmann::json;

// ============================ Config ===========================
namespace config {
    struct Infantry {
        float walking_speed_cps = 4.0f; 
        float walking_speed_spc = 9.0f; // when cell 10m x 10m
        float looking_speed_cps = 4.0f;
        float looking_speed_spc = 9.0f; // currently should be equal to walking speed 
        float fire_freq_rifle = 1.0f;
        float phit_rifle = 0.7f;
        float pkill_open = 0.35f; // https://www.jasss.org/18/4/10.html#fig1 !!!!!!!!!!unused
        float pkill_covered_rifle = 0.1f; // https://www.jasss.org/18/4/10.html#fig1
        float pkill_covered_art = 0.3f;
        int vision = 20;
    };
    struct Artillery {
        float fire_freq_rps = 20.0f; // M777 155mm 3 rpm 위키백과
        float range_error = 5;
        float explosive_range = 5;
        float phit_he = 1.0f;
        float ammo = 10;
    };
    inline Infantry inf;
    inline Artillery art;
}
// ============================ Types ============================
enum class TerrainType { PLAIN, RIVER }; // terrain def
enum class SideType { BLUE, RED }; 
enum class ForceType { RIFLE, ARTILLERY, TANK, DEFAULT };
enum class TaskType { MOVE, BOMBARD, HOLD };
struct Entity { 
    int id;
    std::string name; // -> entity Id 
    SideType side; 
    ForceType forceType; 
    Point position; 
};

// scenario def
struct Scenario {
    int width  = 0;                         // 맵 가로
    int height = 0;                         // 맵 세로
    unsigned int seed = 0;                  // RNG 시드 (0이면 Generator가 생성)

    std::vector<std::pair<float, Rect>> goalRects;    // first: score weight, second: area rect
    std::vector<std::pair<TerrainType, Rect>> terrainRects; // first: terrain type, second: area rect
    std::vector<Entity> entities;

};

// order def
struct Order {
    TaskType task = TaskType::HOLD;
    Point to{0, 0};
    bool hasDestination = false;
};

struct ScoringWeights {
    static constexpr double ALLY = 1.0;
    static constexpr double BOTH = 0.5;
    static constexpr double ENEMY = 0.0;
};
// column headers: seed, blueInit, redInit, blueCasualties, redCasualties, bg control info, totalScore
struct Result{
    unsigned int seed = 0;
    int blueInit = 0;
    int redInit = 0;
    int blueCasualties = 0;
    int redCasualties = 0;
    std::vector<int> goalBlueCount; // blue units per goal area
    std::vector<int> goalRedCount;  // red units per goal area
    std::vector<float> goalScore; // 각 목표 지역별로 얻은 점수
    float totalScore = 0.0;
};
enum class EnvMoveResponse {
    Accepted,
    NotFound,        // id 없음
    OutOfBounds,     // 지도 밖
    InvalidTerrain  // 이동 불가 지형
};
enum class EnvKillResponse {
    Accepted,
    NotFound        // id 없음
};

// ============================ Messages ============================

// EF
class StartMsg{
public:
    Scenario* scen;
    // todo : ef한테만 보내면 되니까 나중에 ef info 같은 메시지로 분리시켜야함
    int experimentIndex = -1; // 몇 번째 실험인지
    float simulationEndTime = 0.0f; // 시뮬레이션 종료 시간
};
class RestartMsg{
public:
    bool needChangeSeed;
    bool needChangeScenario;
};
class ResultMsg{
public:
    Result* res;
};

class CompanyOrd{
public:
    std::unordered_map<int,std::vector<Order>> orders; 
};
class PlatoonOrd{
public:
    std::unordered_map<int,Order> orders;
};

// === REPORT ===
class PlatoonRep{
public:
    int entityId; // sender
    bool succeed;
    // current Platoon Position
    // # of dead men
};
class SoldierRep{
public:
    int entityId; // sender
    bool enemyDetected;
    std::vector<int>* enemyIds;
};


// === Soldier ===
class PositionMsg{
public:
    int senderId;
    Point curPos;
};
class FireMsg{
public:
    int senderId;
    ForceType senderType;
    int targetId;
    std::vector<Point> targetPoint;
    float explosive_pkill;
};

class DeadMsg{
public:
};
