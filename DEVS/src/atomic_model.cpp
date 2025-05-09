#include "atomic_model.h"
#include "engine.h"
#include <iostream>

AtomicModel::AtomicModel(int modelID){
    SetModelID(modelID);
    time = 0;
}

// Ref: System Modeling Simulation, KTG, 108p 2-6-8
void AtomicModel::HandleExtEvent(const Event& extEvent, const std::string& inPort, float currentTime) {
    continueFlag = false;
    time = currentTime;
    
    ExtTransFn(inPort, extEvent.GetMessage());
    if (!continueFlag) {
        ExecTimeAdvance();
    }
}

void AtomicModel::HandleTimeAdvance() {
    OutputFn();
    IntTransFn();
    ExecTimeAdvance();
}

void AtomicModel::ContinueTimeAdvance() {
    continueFlag = true;
}

bool AtomicModel::CheckContinue() {
    bool ret = continueFlag;
    continueFlag = false;
    return ret;
}

// 이 부분은 engine 소스를 구현한 후에 다시 작성할 것
void AtomicModel::AddOutputEvent(const std::string& outPort, const std::any& message) {
    if (engine) {
        engine->AddEvent(Event(this, outPort, message, nextTime));
    } else {
        std::cerr << "[AtomicModel] Warning: engine not set for model ID " << GetModelID() << std::endl;
    }
}

void AtomicModel::ExecTimeAdvance() {
    nextTimeAdvance = TimeAdvanceFn();
    nextTime = time + nextTimeAdvance; 
}

float AtomicModel::GetTime() const {
    return time;
}

float AtomicModel::GetNextTimeAdvance() const {
    return nextTimeAdvance;
}

float AtomicModel::GetNextTime() const {
    return nextTime;
}
