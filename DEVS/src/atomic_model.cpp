#include "atomic_model.h"

AtomicModel::AtomicModel(int modelID){
    SetModelID(modelID);
}

// Ref: System Modeling Simulation, KTG, 108p 2-6-8
void AtomicModel::HandleExtEvent(const Event extEvent, const std::string inPort, time_t curTime) {
    ExtTransFn(extEvent.getMessage());
    

}
void AtomicModel::HandleTimeAdvance() {
    OuputFn();
    IntTransFn();
    UpdateTime();
}

const float AtomicModel::getTime() const {
    return time;
}

void AtomicModel::UpdateTime(){
    time = time + TimeAdvanceFn();
}

void AtomicModel::addOutputEvent(){
    
}