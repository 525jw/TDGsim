#pragma once
#include <string>

class FIRE_INFO{
public:
    int targetX;
    int targetY;
    int damage;
    int orgSenderModelID; // DEBUG ONLY
};
class DETECT_REQ_INFO{
public:
    std::pair<int, int> curPos; // 현재 위치 좌표
    std::pair<int, int> tgtPos; // 표적 위치 좌표
};

class DETECT_RTN_INFO{
    public:
    std::pair<int, int> tgtPos; // 표적 위치 좌표
    bool isDetect; // 탐지 여부
};
class MOVE_POS_INFO{
public:
    std::pair<int, int> curPos; // 현재 위치 좌표
    std::pair<int, int> detPos; // 목적지 좌표
};

class CUR_POS{
public:
    std::pair<int, int> pos; // 현재 위치 좌표
};

class M_FLAG{
public:
    bool isStop;
};

class LOG_ORD{
public:
    std::string ack_msg;
};