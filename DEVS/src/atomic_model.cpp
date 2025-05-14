#pragma once
#include "atomic_model.hpp"
#include "engine.hpp"
#include "event.hpp"
#include <algorithm>

// AtomicModel::AtomicModel(int modelID, Engine* engine)
// {
//     SetModelID(modelID);
//     SetEngine(engine);
// }
AtomicModel::AtomicModel(Engine* engine)
{
    SetEngine(engine);
}

void AtomicModel::AddState(const std::string& state){
    this->states.push_back(state);
}
void AtomicModel::RemoveState(const std::string& state){
    this->states.erase(std::remove(this->states.begin(), this->states.end(), state), this->states.end());
}
const std::vector<std::string>& AtomicModel::GetStates() const{
    return this->states;
}

void AtomicModel::ReceiveExternalEvent(const Event& externalEvent,TIME_T engineTime){
    ExtTransFn(externalEvent.getSenderPort(), externalEvent.getMessage());
    UpdateTime(engineTime);
}

void AtomicModel::ReceiveTimeAdvanceRequest(const TIME_T engineTime){
    OutputFn();
    IntTransFn();
    UpdateTime(engineTime);
}
const TIME_T AtomicModel::QueryNextTime() const{
    return this->nextTime;
}

// Ref. 4-3-8
void AtomicModel::UpdateTime(const TIME_T engineTime){
    this->lastTime = engineTime;
    this->nextTime = engineTime + TimeAdvanceFn();
}

void AtomicModel::addOutputEvent(const std::string& outputPort, std::any& message){
    Event event(*this, outputPort, message);
    this->engine->AddEvent(event);
}