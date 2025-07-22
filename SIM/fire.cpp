#include "fire.hpp"
#include <cmath>

Fire::Fire(int modelID, Engine* engine, int objectID)
    : AtomicModel(modelID, engine)
{
    this->objectID = objectID;

    this->AddState("WAIT");
    this->AddState("FIRE");
    this->SetCurState("WAIT");

    this->AddInputPort("fire_in");
    this->AddInputPort("dead_in");
    this->AddOutputPort("fire_result");
}

TIME_T Fire::fireEquation(){
    return this->capability[0].reload_time;
}

bool Fire::ExtTransFn(const std::string& inPort, const std::any& message){
    if (inPort == "fire_in"){
        try{
            const FireIn msg = std::any_cast<FireIn>(message);

            this->targetID = msg.targetID;

            this->t_fire = fireEquation();

            this->SetCurState("FIRE");
        }catch (const std::bad_any_cast&){
            logger_system << "[ERROR] Invalid message! port name : fire_in\n";
            return false;
        }
    }else if (inPort == "dead_in"){
        this->SetCurState("WAIT");
    }

    return true;
}

bool Fire::IntTransFn(){
    if (this->GetCurState() == "FIRE")
        this->SetCurState("WAIT");
    return true;
}

bool Fire::OutputFn(){
    if (this->GetCurState() == "FIRE"){
        FireResult msg_raw;
        msg_raw.targetID = this->targetID;

        std::any msg_final = msg_raw;
        this->AddOutputEvent("fire_result", msg_final);
    }
    return true;
}

TIME_T Fire::TimeAdvanceFn(){
    if (this->GetCurState() == "WAIT") return TIME_INF;
    if (this->GetCurState() == "FIRE") return t_fire;

    return -1;
}

void Fire::UpdateTime(const TIME_T currentTime){
    this->lastTime = currentTime;
    this->nextTime = currentTime + TimeAdvanceFn();
}

void Fire::ReceiveScheduleTime(const TIME_T currentTime){
    AtomicModel::ReceiveScheduleTime(currentTime);
}

void Fire::ReceiveEvent(Event& event, TIME_T currentTime){
    AtomicModel::ReceiveEvent(event, currentTime);
}

const TIME_T Fire::QueryNextTime() const{
    return AtomicModel::QueryNextTime();
}
