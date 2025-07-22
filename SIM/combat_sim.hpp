#pragma once
#include "../DEVS/coupled_model.hpp"
#include "../DEVS/logger.hpp"

#include "generator.hpp"
#include "data_collector.hpp"

#include <string>

class CombatSim : public CoupledModel{
public:
    std::string myName;
    CombatSim(int modelID, Engine* engine, std::string name);
    void ReceiveScheduleTime(const TIME_T currentTime) override;
    void ReceiveEvent(Event& event, TIME_T currentTime) override;
    const TIME_T QueryNextTime() const override;
};