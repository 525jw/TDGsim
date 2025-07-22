//WIP
#pragma once
#include "atomic_model.hpp"
#include "../message.hpp"
#include "../DEVS/logger.hpp"
#include <utility>
#include <iostream>
#include <string>

class Environment : public AtomicModel{
private:
    std::unordered_map<std::string, std::pair<int, int>> redteam;    // key : objectID, value : (x,y)
    std::unordered_map<std::string, std::pair<int, int>> blueteam;

    std::vector<std::vector<int>> grid_map;
public:
    Environment(int modelID, Engine* engine, std::string name);
    bool ExtTransFn(const std::string& inPort, const std::any& message);
    bool IntTransFn();
    bool OutputFn();
    TIME_T TimeAdvanceFn();
    void UpdateTime(const TIME_T currentTime);
    void ReceiveScheduleTime(const TIME_T currentTime) override;
    void ReceiveEvent(Event& event,TIME_T currentTime) override;
    const TIME_T QueryNextTime() const override;
};