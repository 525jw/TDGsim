#include "damage_evaluation.hpp"
#include <cmath>

DamageEvaluation::DamageEvaluation(int modelID, Engine* engine, int objectID)
    : AtomicModel(modelID, engine)
{
    this->objectID = objectID;

    this->AddState("WAIT");
    this->AddState("EVAL");
    this->SetCurState("WAIT");

    this->AddInputPort("fire_result");
    this->AddOutputPort("dead_out");
}

bool DamageEvaluation::ExtTransFn(const std::string& inPort, const std::any& message){
    if (inPort == "fire_result")
    {
        try{
            const FireResult msg = std::any_cast<FireResult>(message);

            if (msg.targetID == this->objectID){
                this->SetCurState("EVAL");
            }
        }
        catch (const std::bad_any_cast&){
            logger_system << "[ERROR] Invalid message! port name : fire_result\n";
            return false;
        }
    }

    return true;
}

bool DamageEvaluation::IntTransFn(){
    if (this->GetCurState() == "EVAL")
        this->SetCurState("WAIT");

    return true;
}

bool DamageEvaluation::OutputFn(){
    if (this->GetCurState() == "EVAL"){
        DeadMsg msg_raw;
        //
        std::any msg_final = msg_raw;
        this->AddOutputEvent("dead_out", msg_final);
    }
    return true;
}

TIME_T DamageEvaluation::TimeAdvanceFn(){
    if (this->GetCurState() == "WAIT") return TIME_INF;
    if (this->GetCurState() == "EVAL") return 0.0f;

    return -1;
}

void DamageEvaluation::UpdateTime(const TIME_T currentTime){
    this->lastTime = currentTime;
    this->nextTime = currentTime + TimeAdvanceFn();
}

void DamageEvaluation::ReceiveScheduleTime(const TIME_T currentTime){
    AtomicModel::ReceiveScheduleTime(currentTime);
}

void DamageEvaluation::ReceiveEvent(Event& event, TIME_T currentTime){
    AtomicModel::ReceiveEvent(event, currentTime);
}

const TIME_T DamageEvaluation::QueryNextTime() const{
    return AtomicModel::QueryNextTime();
}
