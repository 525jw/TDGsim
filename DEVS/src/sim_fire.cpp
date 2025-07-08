#include "sim_fire.hpp"

Fire::Fire(int modelID, Engine* engine, std::string name)
    : AtomicModel(modelID, engine)
{
    this->myName = name;

    this->AddState("wait");
    this->AddState("process");
    this->AddState("report");

    this->SetCurState("wait");

    logger_world    << "["
                    << myName
                    << "]"
                    <<" Created"
                    <<std::endl;

    logger_system   << "[" << "Fire::"
                    << myName
                    << ":Init]"
                    <<" Created, ID : "<<this->GetModelID()
                    <<", current State : "<<this->GetCurState()
                    <<std::endl;
}

bool Fire::ExtTransFn(const std::string& inPort, const std::any& message) 
{
}

bool Fire::IntTransFn() 
{
}

bool Fire::OutputFn() 
{
}

TIME_T Fire::TimeAdvanceFn() 
{
}

// logging only
void Fire::UpdateTime(const TIME_T currentTime){
    this->lastTime = currentTime;
    this->nextTime = currentTime + TimeAdvanceFn();
    logger_system   << "[" << "Fire::"
                    << myName
                    << "::UpdateTime]"
                    << " LastTIme, NextTime = ("<<this->lastTime<<","<<this->nextTime<<")"
                    << std::endl;
}
void Fire::ReceiveScheduleTime(const TIME_T currentTime) {
    AtomicModel::ReceiveScheduleTime(currentTime);
    logger_system   << "[" << "Fire::"
                    << myName
                    << "::ReceiveScheduleTime]"
                    <<" Received (*,"<<currentTime<<"), next TA updated to "<<this->nextTime
                    <<std::endl;
}
void Fire::ReceiveEvent(Event& event,TIME_T currentTime) {
    AtomicModel::ReceiveEvent(event, currentTime);
    logger_system   << "[" << "Fire::"
                    << myName
                    << "::ReceiveEvent]"
                    <<" Received (x,"<<currentTime<<"), next TA updated to "<<this->nextTime
                    << std::endl;
}
const TIME_T Fire::QueryNextTime() const {
    logger_system   << "[" << "Fire::"
                    << myName
                    << "::QueryNextTime]"
                    <<" sends TA : "<<this->nextTime
                    << std::endl;
    return AtomicModel::QueryNextTime();
}