#pragma once
#include "atomic_model.hpp"
#include "sim_environment.hpp"
#include "message.hpp"
#include "logger.hpp"
#include <utility>
#include <iostream>
#include <string>

class DecisionMaker : public AtomicModel{
    private:
    // --- 표적 리스트 ---
    std::vector<std::pair<int, int>> targetList;
    public:
    std::string myName;

    std::pair<int, int> curPos; // 현재 위치
    std::pair<int, int> detPos; // 목표 지점 (이동이건 표적이건)


    DecisionMaker(int modelID, Engine* engine, std::string name);
    bool ExtTransFn(const std::string& inPort, const std::any& message);
    bool IntTransFn();
    bool OutputFn();


    TIME_T TimeAdvanceFn();
    void UpdateTime(const TIME_T currentTime);
    void ReceiveScheduleTime(const TIME_T currentTime) override;
    void ReceiveEvent(Event& event,TIME_T currentTime) override;
    const TIME_T QueryNextTime() const override;
};