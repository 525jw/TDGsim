#pragma once
#include "DEVS/atomic_model.hpp"
#include "message.hpp"
#include "DEVS/logger.hpp"
#include <utility>
#include <iostream>
#include <vector>
#include <any>

class DecisionMaker : public AtomicModel{
private:
    int objectID;

    std::pair<int,int> curPos={-1,-1};
    std::pair<int,int> detPos={-1,-1};
    std::unordered_map<int, std::pair<int,int>> enemyPos;

    TIME_T t_dc = TIME_INF;
public:
    DecisionMaker(Engine* engine, int objectID);

    bool ExtTransFn(const std::string& inPort, const std::any& anyMessage);
    bool IntTransFn();
    bool OutputFn();
    TIME_T TimeAdvanceFn();
};