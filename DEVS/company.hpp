#pragma once
#include "coupled_model.hpp"
#include "logger.hpp"



#include <string>

class Company : public CoupledModel{
public:
    std::string myName;
    Company(int modelID, Engine* engine, std::string name);
    void ReceiveScheduleTime(const TIME_T currentTime) override;
    void ReceiveEvent(Event& event, TIME_T currentTime) override;
    const TIME_T QueryNextTime() const override;
};