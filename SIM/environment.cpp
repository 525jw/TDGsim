//WIP
#include "environment.hpp"

Environment::Environment(int modelID, Engine* engine, std::string name)
    : AtomicModel(modelID, engine)
{
    this->AddState("IDLE");
    this->AddState("UPDATE");

    this->SetCurState("IDLE");

    this->AddInputPort("scen_info");
    this->AddInputPort("red_pos");
    this->AddInputPort("blue_pos");
    this->AddInputPort("dead_in");

    this->AddOutputPort("env_info");
    this->AddOutputPort("red_pos");
    this->AddOutputPort("blue_pos");
}

bool Environment::ExtTransFn(const std::string& inPort, const std::any& message) {

    return true;
}

bool Environment::IntTransFn() {

    return true;
}

bool Environment::OutputFn() {

    return true;
}

TIME_T Environment::TimeAdvanceFn() {
    if (this->GetCurState() == "WAIT") return TIME_INF;
    if (this->GetCurState() == "UPDATE") return 0.0f;
    return -1;
}

void Environment::UpdateTime(const TIME_T currentTime){
    this->lastTime = currentTime;
    this->nextTime = currentTime + TimeAdvanceFn();
}

void Environment::ReceiveScheduleTime(const TIME_T currentTime) {
    AtomicModel::ReceiveScheduleTime(currentTime);
}

void Environment::ReceiveEvent(Event& event, TIME_T currentTime) {
    AtomicModel::ReceiveEvent(event, currentTime);
}

const TIME_T Environment::QueryNextTime() const {
    return AtomicModel::QueryNextTime();
}
