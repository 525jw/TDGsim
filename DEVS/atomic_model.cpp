#include "atomic_model.hpp"

AtomicModel::AtomicModel(Engine* engine)
    : Model(engine) {}
    
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


void AtomicModel::ReceiveEvent(Event& event,TIME_T currentTime){
    if(this->lastTime <= currentTime && currentTime <= this->nextTime){
        this->executedTime = currentTime - this->lastTime;
        ExtTransFn(event.getSenderPort(), event.getMessage());
        UpdateTime(currentTime);
    }else{
        // event는 free 되어야함 (invalid event)
    }
}

void AtomicModel::ReceiveScheduleTime(const TIME_T currentTime){
    if(currentTime == this->nextTime){
        OutputFn();
        IntTransFn();
        UpdateTime(currentTime);
    }else{
        // ERROR
    }
}
const TIME_T AtomicModel::QueryNextTime() const{
    if (this->GetNextTime() <= 100000.0f) {
        logger_system << "[MinTA] " << this->GetModelID()
                        << " state : "<<this->GetCurState()
                        << " : next time -> " << this->GetNextTime() << std::endl;
    }
    return this->nextTime;
}

// Ref. 4-3-8
void AtomicModel::UpdateTime(const TIME_T currentTime){
    this->lastTime = currentTime;
    this->nextTime = currentTime + TimeAdvanceFn();
}

// Only Called in OutputFn()
void AtomicModel::AddOutputEvent(const std::string& outputPort, std::any& message){
    Event* event = new Event(this->GetModelID(), outputPort, message);
    this->engine->AddEvent(event);
}