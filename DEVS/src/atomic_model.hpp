#pragma once
#include "model.hpp"
#include "event.hpp"
#include <any>
#include <string>

class Engine;

class AtomicModel : public Model{
private:
    std::vector<std::string> states;
    std::string currentState;
public:
    // AtomicModel(int modelID, Engine* engine);
    AtomicModel(Engine* engine);

    void AddState(const std::string& state);
    void RemoveState(const std::string& state);
    const std::vector<std::string>& GetStates() const;
    void SetCurState(std::string state);
    const std::string& GetCurState() const;

    void ReceiveExternalEvent(const Event& externalEvent, TIME_T engineTime);
    void ReceiveTimeAdvanceRequest(const TIME_T engineTime);
    const TIME_T QueryNextTime() const;

    void UpdateTime(const TIME_T engineTime);
    void addOutputEvent(const std::string& outputPort, std::any& message);

    virtual bool ExtTransFn(const std::string& inPort, const std::any& message) {return false;}
    virtual bool IntTransFn() {return false;}
    virtual bool OutputFn() {return false;}
    virtual TIME_T TimeAdvanceFn() {return -1;}
};


/* 
ajw
ExtTransFn에 모든 상태천이 판단을 맡김
-> 그렇다면 HandleExtEvent-ExtTransFn으로 어떻게 message를 전달할 것인가
1. HandleExtEvent 호출 시 Event를 직접 넘겨준다
2. engine에서 broadcast시 Model 내부의 버퍼에 message 정보를 담고, ExtTransFn에서는
    buffer를 들여다보고 판단.
*/