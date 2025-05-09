#pragma once
#include "model.h"
#include "event.h"
#include <any>
#include <string>

class AtomicModel : public Model{
private:
    float time = 0.0f;              // 현재 시각
    float nextTimeAdvance = 0.0f;   // 다음 전이까지 걸리는 시간
    float nextTime = 0.0f;          // 다음 전이 시각
    bool continueFlag = false;      // 외부 천이 후 내부 시간전진 할지말지...

public:
    AtomicModel(int modelID);

    // 각 모델이 오버라이드 해야 하는 함수들
    virtual bool ExtTransFn(const std::string& port, const std::any& message) {return false;}    // 외부 전이 함수
    virtual bool IntTransFn() {return false;}       // 내부 전이 함수
    virtual bool OutputFn() {return false;}         // 출력 함수
    virtual float TimeAdvanceFn() {return -1.0f;}   // 시간 전진 함수

    // 외부 이벤트 수신 및 처리
    void HandleExtEvent(const Event& extEvent, const std::string& inPort, float currentTime);
    // 시간 전진 요청 처리
    void HandleTimeAdvance();
    
    void ContinueTimeAdvance();
    bool CheckContinue();

    void AddOutputEvent(const std::string& outPort, const std::any& message);
    void ExecTimeAdvance();

    float GetTime() const;
    float GetNextTimeAdvance() const;
    float GetNextTime() const;
};


/* 
ajw
ExtTransFn에 모든 상태천이 판단을 맡김
-> 그렇다면 HandleExtEvent-ExtTransFn으로 어떻게 message를 전달할 것인가
1. HandleExtEvent 호출 시 Event를 직접 넘겨준다
2. engine에서 broadcast시 Model 내부의 버퍼에 message 정보를 담고, ExtTransFn에서는
    buffer를 들여다보고 판단.
*/