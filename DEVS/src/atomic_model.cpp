#pragma once
#include "atomic_model.hpp"
#include "engine.hpp"
#include "event.hpp"
#include <algorithm>

AtomicModel::AtomicModel(int modelID, Engine* engine)
{
    SetModelID(modelID);
    SetEngine(engine);
    this->engine->RegisterModelWithID(this);
}

const std::vector<std::string>& AtomicModel::GetStates() const{
    return this->states;
}
void AtomicModel::SetCurState(std::string state) {
    this->currentState = state;
}
const std::string& AtomicModel::GetCurState() const {
    return this->currentState;
}
void AtomicModel::AddState(const std::string& state) {
    this->states.push_back(state);
}
void AtomicModel::RemoveState(const std::string& state) {
    this->states.erase(
        std::remove(this->states.begin(), this->states.end(), state),
        this->states.end()
    );
}


void AtomicModel::ReceiveExternalEvent(const Event& externalEvent,TIME_T engineTime){
    if(this->lastTime <= engineTime && engineTime <= this->nextTime){
        // this->executedTime = engineTime - this->lastTime;
        ExtTransFn(externalEvent.getSenderPort(), externalEvent.getMessage());
        UpdateTime(engineTime);
    }else{
        // ERROR
    }
}

void AtomicModel::ReceiveTimeAdvanceRequest(const TIME_T engineTime){
    if(engineTime == this->nextTime){ // if t=tN then
        OutputFn();
        IntTransFn();
        UpdateTime(engineTime);
    }else{
        // ERROR
    }
}
const TIME_T AtomicModel::QueryNextTime() const{
    return this->nextTime;
}

// Ref. 4-3-8
void AtomicModel::UpdateTime(const TIME_T engineTime){
    this->lastTime = engineTime;
    this->nextTime = engineTime + TimeAdvanceFn();
}

void AtomicModel::AddOutputEvent(const std::string& outputPort, std::any& message){
    Event* event = new Event(*this, outputPort, message);
    this->engine->AddEvent(event);
}