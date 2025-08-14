#pragma once
#include <utility>
#include <vector>

enum class TerrainType { PLAIN, RIVER, ROAD, FOREST, HILL};
enum class ForceType { RED_INF, BLUE_INF, RED_ARM, BLUE_ARM, BLUE_ };

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

class BluePosINF{
public:
    int entityID;
    std::pair<int,int> curPos;
};
// class MnvOrd{
// public:
//     float speed; // m per s
//     std::pair<int,int> nextPos; // (x,y)
// };
// class MnvRes{
// public:
//     int entityID;
//     std::pair<int,int> curPos; // (x,y)
// };

// class DetRes{
// public:
//     bool enemyDetected;
//     std::unordered_map<int, std::pair<int,int>> enemyPos; // key: objectID, value:(x,y)
// };

// class FireIn{ // CapabilityInfo
// public:
//     // capability details
//     int targetID;
// };
// class FireResult{
// public:
//     // damage details
//     int targetID;
// };

// class DeadInfo{
// public:
//     int entityID;
// };

/*
=== ORDER ===
std::pair<int,int> detPos 
- 보병,기갑 : 기동지점
- 포병 : 사격지점
*/
struct CompanyOrdType{ std::pair<int,int> detPos; TIME_T deadline; };
class CompanyOrd{
public:
    std::unordered_map<int, CompanyOrdType> orders; // key : entityID, value : detPos, deadline
};
class PlatoonOrd{
public:
    std::pair<int,int> detPos;
};

// === REPORT ===
class CompanyRep{
public:
    bool succeed;
};

class PlatoonRep{
public:
    int entityID;
    bool succeed;
};

class SoldierRep{
public:
    int entityID;
    bool enemyDetected;
};