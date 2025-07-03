#pragma once
#include "atomic_model.hpp"
#include "sim_environment.hpp"
#include "message.hpp"
#include "logger.hpp"
#include <utility>
#include <iostream>
#include <string>

class Maneuver : public AtomicModel{
public:
    int health = 100;
    int damageCount = 0;
    int fireCount = 0;
    std::pair<int, int> myPosXY;
    std::string myName;

    // --- 이동 관련 변수 ---
    std::pair<int, int> detPos; // 목적지 좌표
    double moveSpeed = 1.0;        // 이동 속도 (초당 몇 칸 등)
    int direction = 0; // x좌표가 움직임(= 가로 방향), 0이면 -, 1이면 +
    bool isMoving = false;         // 이동 중 여부


    Maneuver(int modelID, Engine* engine, std::string name);
    bool ExtTransFn(const std::string& inPort, const std::any& message);
    bool IntTransFn();
    bool OutputFn();

    // --- 이동 관련 함수 ---


    TIME_T TimeAdvanceFn();
    void UpdateTime(const TIME_T currentTime);
    void ReceiveScheduleTime(const TIME_T currentTime) override;
    void ReceiveEvent(Event& event,TIME_T currentTime) override;
    const TIME_T QueryNextTime() const override;

    
};