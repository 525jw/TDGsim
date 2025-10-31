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
#include "json.hpp"
using json = nlohmann::json;
// ============================ Config ===========================
namespace config {
    struct Infantry {
        // float walking_speed_cps = 4.0f; 
        float walking_speed_spc = 9.0f; // when cell 10m x 10m
        // float looking_speed_cps = 4.0f;
        float looking_speed_spc = 9.0f; // currently should be equal to walking speed 
        float fire_freq_rifle = 3.0f;
        float fire_accuracy_rifle = 0.7f;
        float pkill_open = 0.35f; // https://www.jasss.org/18/4/10.html#fig1
        float pkill_covered = 0.1f; // https://www.jasss.org/18/4/10.html#fig1
        int vision = 20;
    };
    struct Artillery {
        float fire_freq_rps = 20.0f; // M777 155mm 3 rpm 위키백과
        // 반지름 5의 원을 그린다
        float explosive_range = 5;
        // 그 중 한 점을 고른다
        float explosive_power = 5;
        float ammo = 10;
    };
    inline Infantry inf;
    inline Artillery art;
}
// ============================ Types ============================

// coordinate def
typedef struct { int x,y; } Point;
inline bool operator==(const Point& a, const Point& b) {
    return a.x == b.x && a.y == b.y;
}
inline bool operator!=(const Point& a, const Point& b) {
    return !(a == b);
}

// terrain def
enum class TerrainType { PLAIN, RIVER };
struct TerrainRect {
    TerrainType kind;   //
    int x1, y1, x2, y2; // 범위
};

// entity def
enum class Side { BLUE, RED };
enum class ForceType { RIFLE, ARTILLERY, TANK };
struct Entity { 
    int id;
    std::string name; // -> entity Id 
    Side side; 
    ForceType forceType; 
    Point position; 
};

// scenario def
struct Scenario {
    int width  = 0;                         // 맵 가로
    int height = 0;                         // 맵 세로
    unsigned int seed = 0;                  // RNG 시드 (0이면 Generator가 생성)

    std::vector<TerrainRect> terrainRects;
    std::vector<Entity> entities;
};

// order def
enum class TaskType { MOVE, BOMBARD, HOLD };
struct Order {
    TaskType task = TaskType::HOLD;
    Point to{0, 0};
    bool hasDestination = false;
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
class Start{
public:
    Scenario* scen;
};
class Restart{
public:
    // not implemented
};
class Result{
public:
    // not implemented
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
};

class DeadMsg{
public:
};
