#pragma once
#include "DEVS/atomic_model.hpp"
#include "DEVS/logger.hpp"
#include "message.hpp"
#include <utility>
#include <iostream>
#include <string>

class Generator : public AtomicModel{
public:
    std::string myName;
    Generator(int modelID, Engine* engine, std::string name);
    bool ExtTransFn(const std::string& inPort, const std::any& message);
    bool IntTransFn();
    bool OutputFn();
    TIME_T TimeAdvanceFn();
    void UpdateTime(const TIME_T currentTime);
    void ReceiveScheduleTime(const TIME_T currentTime) override;
    void ReceiveEvent(Event& event,TIME_T currentTime) override;
    const TIME_T QueryNextTime() const override;
};