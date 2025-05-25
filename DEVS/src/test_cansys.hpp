#pragma once
#include "coupled_model.hpp"
#include "test_canteam.hpp"
#include "logger.hpp"

class Engine;

class TestCannonSys : public CoupledModel{
private:
    std::string myName;
public:
    TestCannonSys(int modelID, Engine* engine);
    void ReceiveScheduleTime(const TIME_T currentTime) override;
    const TIME_T QueryNextTime() const override;
    void ReceiveEvent(Event& externalEvent, TIME_T currentTime);
};