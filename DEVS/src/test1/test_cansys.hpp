#pragma once
#include "coupled_model.hpp"
#include "test_canteam.hpp"
#include "logger.hpp"

class Engine;

class TestCannonSys : public CoupledModel{
public:
    std::string myName;
    TestCannonSys(int modelID, Engine* engine);
    void ReceiveScheduleTime(const TIME_T currentTime) override;
    const TIME_T QueryNextTime() const override;
    void ReceiveEvent(Event& externalEvent, TIME_T currentTime);
};