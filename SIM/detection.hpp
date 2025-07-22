#pragma once
#include "DEVS/atomic_model.hpp"
#include "message.hpp"
#include "DEVS/logger.hpp"
#include <utility>
#include <iostream>
#include <vector>
#include <any>

class Detection : public AtomicModel{
private:
    int objectID;

    std::pair<int,int> cur_pos={-1, -1}; // (x,y)
    std::unordered_map<int, std::pair<int,int>> enemy_pos;   // key: Enemy objectID, value: enemy_pos (x,y)
    int vision = 10;

    TIME_T t_det = TIME_INF;
    void   RebuildEnemyDistance(const EnvInfo& info);
    TIME_T detEquation() const;
public:
    Detection(int modelID, Engine* engine, int objectID);

    bool ExtTransFn(const std::string& inPort, const std::any& message);
    bool IntTransFn();
    bool OutputFn();
    TIME_T TimeAdvanceFn();

    void UpdateTime(const TIME_T currentTime);
    void ReceiveScheduleTime(const TIME_T currentTime) override;
    void ReceiveEvent(Event& event,TIME_T currentTime) override;
    const TIME_T QueryNextTime() const override;
};