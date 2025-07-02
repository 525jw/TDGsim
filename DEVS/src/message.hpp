#pragma once
#include <string>

class FIRE_INFO{
public:
    int targetX;
    int targetY;
    int damage;
    int orgSenderModelID; // DEBUG ONLY
};

class LOG_ORD{
public:
    std::string ack_msg;
};