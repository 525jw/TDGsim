#pragma once
#include <utility>
#include <vector>

enum class Team { RED, BLUE };
enum class TerrainType { PLAIN, MOUNTAIN, RIVER };

class ScenInfo{
public:
    unsigned int seed;
    unsigned int mapSize;
    std::unordered_map<int, std::pair<int, int>> blueTeam;
    std::unordered_map<int, std::pair<int, int>> redTeam;
};
// class RestartMsg{
// public:
// };

class EnvInfo{
public:
    std::unordered_map<int, std::pair<int, int>> blueTeam;
    std::unordered_map<int, std::pair<int, int>> redTeam;
};

class MnvOrd{
public:
    float speed; // m per s
    std::pair<int,int> nextPos; // (x,y)
};
class MnvRes{
public:
    int objectID;
    std::pair<int,int> curPos; // (x,y)
    Team team;
};

class DetRes{
public:
    bool enemyDetected;
    std::unordered_map<int, std::pair<int,int>> enemyPos; // key: objectID, value:(x,y)
};

class FireIn{ // CapabilityInfo
public:
    // capability details
    int targetID;
};
class FireResult{
public:
    // damage details
    int targetID;
};

class DeadInfo{
public:
    int objectID;
    Team team;
};