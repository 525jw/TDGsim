#pragma once
#include "DEVS/coupled_model.hpp"
#include "DEVS/logger.hpp"

#include "company.hpp"

#include <string>

class RedForce : public CoupledModel{
public:
    std::string myName;
    RedForce(int modelID, Engine* engine, std::string name);
    void ReceiveScheduleTime(const TIME_T currentTime) override;
    void ReceiveEvent(Event& event, TIME_T currentTime) override;
    const TIME_T QueryNextTime() const override;
};