#pragma once
#include <utility>
#include <vector>

typedef struct { int x,y; } Point;
inline bool operator==(const Point& a, const Point& b) {
    return a.x == b.x && a.y == b.y;
}
inline bool operator!=(const Point& a, const Point& b) {
    return !(a == b);
}
enum class Team { BLUE, RED };
enum class TerrainType { PLAIN, RIVER };
enum class ForceType { RIFLE, ARTILLERY };
struct Entity { 
    Team team; 
    ForceType forceType; 
    Point position; 
    std::string name; 
};
// ====== Environment ======
class EnvMsg{
public:
    // ENV is currently declared as a global instance
};
enum class EnvMoveResponse {
    Accepted,
    NoOp,            // 같은 자리
    NotFound,        // id 없음
    OutOfBounds,     // 지도 밖
    OccupiedOther,   // 타 엔티티 점유
    InvalidTerrain  // 이동 불가 지형
};
enum class EnvKillResponse {
    Accepted,
    NotFound        // id 없음
};


// ====== EF ======
class Start{
public:
    unsigned int seed;
};
class Restart{
public:
    // not implemented
};
class Result{
public:
    // not implemented
};



// ====== Order ======
enum class TaskType { MOVE, BOMBARD };
typedef struct {
    TaskType task; // task
    Point to; // to
} Order;
class CompanyOrd{
public:
    std::unordered_map<int,Order> orders;
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
    Point targetPoint;
};