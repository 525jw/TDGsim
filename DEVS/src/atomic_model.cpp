#pragma once
#include "atomic_model.hpp"
#include "engine.hpp"
#include "event.hpp"

AtomicModel::AtomicModel(int modelID){
    SetModelID(modelID);
    this->time = 0.0;
    this->nextTime = INF_FLOAT;
}

// Ref: System Modeling Simulation, KTG, 108p 2-6-8
void AtomicModel::HandleExtEvent(const Event extEvent, const std::string inPort, TIME_T engineTime) {
    ExtTransFn(extEvent.getMessage());
    this->time = engineTime;
    UpdateNextTime();
}
void AtomicModel::HandleTimeAdvance(const TIME_T engineTime) {
    this->time = engineTime;
    OutputFn();
    IntTransFn();
    UpdateNextTime();
}

void AtomicModel::UpdateNextTime(){
    this->nextTime = this->time + TimeAdvanceFn();
}

void AtomicModel::addOutputEvent(const std::string outPort, std::any* message){
    Event event(*this, outPort, message);
    this->engine->AddEvent(event);
}

const float AtomicModel::queryNextTime() const {
    return this->nextTime;
}
