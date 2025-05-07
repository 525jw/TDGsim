#pragma once
#include "model.h"
#include "event.h"
#include <any>
#include <string>

class AtomicModel : public Model{
private:
    float time=0;
public:
    AtomicModel(int modelID);

    virtual bool ExtTransFn(const std::any& message) {return false;}
    virtual bool IntTransFn() {return false;}
    virtual bool OuputFn() {return false;}
    virtual float TimeAdvanceFn() {return -1;}

    void HandleExtEvent(const Event extEvent, const std::string inPort, time_t curTime);
    void HandleTimeAdvance();
    void UpdateTime();
    const float getTime() const;
    void addOutputEvent();
};


/* 
ajw
ExtTransFn에 모든 상태천이 판단을 맡김
-> 그렇다면 HandleExtEvent-ExtTransFn으로 어떻게 message를 전달할 것인가
1. HandleExtEvent 호출 시 Event를 직접 넘겨준다
2. engine에서 broadcast시 Model 내부의 버퍼에 message 정보를 담고, ExtTransFn에서는
    buffer를 들여다보고 판단.
*/