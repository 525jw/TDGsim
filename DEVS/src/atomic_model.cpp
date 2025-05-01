#include "atomic_model.h"

AtomicModel::AtomicModel(int modelID){
    SetModelID(modelID);
}

// Ref: System Modeling Simulation, KTG, 108p 2-6-8
bool AtomicModel::HandleExtEvent(const std::string inPort, time_t curTime) {
    
    return false;
}
bool AtomicModel::HandleTimeAdvance() {
    return false;
}
bool AtomicModel::ExtTransFn(const std::any& message) {
    return false;
}
bool AtomicModel::IntTransFn() {
    return false;
}
bool AtomicModel::OuputFn() {return false;}
time_t TimeAdvanceFn() {return -1;}

const time_t AtomicModel::getTime() const {
    return time;
}
void ReqEnqueueEvent(){
    
}