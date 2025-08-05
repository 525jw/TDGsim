#pragma once
#include "DEVS/atomic_model.hpp"
#include "message.hpp"
#include "DEVS/logger.hpp"
#include <utility>
#include <iostream>

class Maneuver : public AtomicModel{
private:
    int objectID;

    std::pair<int,int> curPos={-1, -1}; // (x,y)
    float curSpeed = -1.0f;

    TIME_T t_mnv = -1.0f;
    TIME_T mnvEquation(float speed);
public:
    Maneuver(Engine* engine, int objectID);

    bool ExtTransFn(const std::string& inPort, const std::any& anyMessage);
    bool IntTransFn();
    bool OutputFn();
    TIME_T TimeAdvanceFn();

    void UpdateTime(const TIME_T currentTime);
    void ReceiveScheduleTime(const TIME_T currentTime) override;
    void ReceiveEvent(Event& event,TIME_T currentTime) override;
    const TIME_T QueryNextTime() const override;
};