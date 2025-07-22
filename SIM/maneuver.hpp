#pragma once
#include "atomic_model.hpp"
#include "../message.hpp"
#include "../DEVS/logger.hpp"
#include <utility>
#include <iostream>

class Maneuver : public AtomicModel{
private:
    int objectID;

    std::pair<int,int> cur_pos={-1.0f, -1.0f}; // (x,y)
    float cur_speed = -1.0f;

    TIME_T t_mnv = -1.0f;
    TIME_T mnvEquation(float speed);
public:
    Maneuver(int modelID, Engine* engine, int objectID);

    bool ExtTransFn(const std::string& inPort, const std::any& message);
    bool IntTransFn();
    bool OutputFn();
    TIME_T TimeAdvanceFn();

    void UpdateTime(const TIME_T currentTime);
    void ReceiveScheduleTime(const TIME_T currentTime) override;
    void ReceiveEvent(Event& event,TIME_T currentTime) override;
    const TIME_T QueryNextTime() const override;
};