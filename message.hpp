#pragma once
#include <utility>
#include <vector>

class MnvOrd{
public:
    float speed; // m per s
    std::pair<int,int> next_pos; // (x,y)
};
class MnvResult{
public:
    int objectID;
    std::pair<int,int> cur_pos; // (x,y)
};

class EnvInfo{
public:
    std::unordered_map<int, std::pair<int,int>> red_pos; // key: objectID, value:(x,y)
    std::unordered_map<int, std::pair<int,int>> blue_pos; // key: objectID, value:(x,y)
};

class DetResult{
public:
    bool IsDetect;
    std::unordered_map<int, std::pair<int,int>> enemy_pos; // key: objectID, value:(x,y)
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

class DeadMsg{
public:
};