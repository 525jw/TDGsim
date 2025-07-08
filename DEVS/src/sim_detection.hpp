#pragma once
#include "atomic_model.hpp"
#include "sim_environment.hpp"
#include "message.hpp"
#include "logger.hpp"
#include <utility>
#include <iostream>
#include <string>

class Detection : public AtomicModel{
private:
    std::pair<int, int> curPos; // 현재 위치 좌표
    std::pair<int, int> tgtPos; // 표적 위치 좌표
    int viewRange = 2; // 시야 범위 (맨해튼 거리 2)
    int actualRange = 0; // 실제 탐지 범위 (맨해튼 거리)
    bool isDectected = false; // 탐지 여부
    bool isDetecting = false; // 탐지 중 여부

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
