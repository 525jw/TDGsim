#pragma once
#include "model.hpp"
#include "event.hpp"
#include "engine.hpp"
#include "logger.hpp"
#include <algorithm>
#include <any>
#include <string>

class Engine;

class AtomicModel : public Model{
protected:
    TIME_T executedTime; // NOTE : Currently unused; specified in the textbook;
private:
    std::vector<std::string> states;
    std::string currentState;
public:
    AtomicModel(int modelID, Engine* engine);

    void AddState(const std::string& state);
    void RemoveState(const std::string& state);
    const std::vector<std::string>& GetStates() const;
     // DISCUSS : State Transition 진입 시 처리되는 과정이 많다면(ex: *->dead) setter 구현만만으로는 부족, stateTransitionFn() 구현논의 
    void SetCurState(std::string state);
    const std::string& GetCurState() const;

    void ReceiveEvent(Event& event, TIME_T currentTime);
    void ReceiveScheduleTime(const TIME_T currentTime);
    const TIME_T QueryNextTime() const;

    void UpdateTime(const TIME_T currentTime);
    void AddOutputEvent(const std::string& outputPort, std::any& message);

    virtual bool ExtTransFn(const std::string& inPort, const std::any& message) {return false;}
    virtual bool IntTransFn() {return false;}
    virtual bool OutputFn() {return false;}
    virtual TIME_T TimeAdvanceFn() {return -1;}

    bool IsAtomic() const override { return true; }
};