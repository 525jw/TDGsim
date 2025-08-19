#pragma once
#include <utility>
#include <vector>

enum class Team { BLUE, RED };
enum class TerrainType { PLAIN, RIVER, ROAD, FOREST, HILL};
enum class ForceType { RED_INF, BLUE_INF, RED_ARM, BLUE_ARM, BLUE_ };
typedef struct { int x,y; } Point;

class ScenInfo{
public:
    unsigned int seed;
};
class Restart{
public:
    // not implemented
};

class EnvMsg{
public:
    // ENV is currently declared as a global instance
};

class MnvRes{
public:
    int senderId;
    Point curPos;
};
class FireRes{
public:
    int senderId;
    int receiverId;
    bool hit;
};
class DmgRes{
public:
    int senderId;
    bool dead;
};
/*
=== ORDER ===
destination
- 보병,기갑 : 기동지점
- 포병 : 사격지점
*/
struct CompanyOrdType{ Point detPos; TIME_T deadline; };
class CompanyOrd{
public:
    std::unordered_map<int, CompanyOrdType> orders; // key : receiver Id , value : detPos , deadline
};
class PlatoonOrd{
public:
    Point detPos;
};

// === REPORT ===
class CompanyRep{
public:
    bool succeed;
};

class PlatoonRep{
public:
    int entityId;
    bool succeed;
};

class SoldierRep{ // = Detection Result
public:
    int senderId;
    bool enemyDetected;
    std::vector<int>* enemyIds;
};