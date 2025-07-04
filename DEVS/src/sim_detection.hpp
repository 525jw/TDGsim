#pragma once
#include "atomic_model.hpp"
#include "sim_environment.hpp"
#include "message.hpp"
#include "logger.hpp"
#include <utility>
#include <iostream>
#include <string>

class Detection : public AtomicModel{
public:
    std::string myName;
    Detection(int modelID, Engine* engine, std::string name);
    bool ExtTransFn(const std::string& inPort, const std::any& message);
    bool IntTransFn();
    bool OutputFn();
    TIME_T TimeAdvanceFn();
    void UpdateTime(const TIME_T currentTime);
    void ReceiveScheduleTime(const TIME_T currentTime) override;
    void ReceiveEvent(Event& event,TIME_T currentTime) override;
    const TIME_T QueryNextTime() const override;

};
