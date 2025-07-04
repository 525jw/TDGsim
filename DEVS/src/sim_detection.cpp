#include "sim_detection.hpp"

Detection::Detection(int modelID, Engine* engine, std::string name)
    :  AtomicModel(modelID, engine)
{
    this->AddState("wait");
    this->AddState("detect");

    this->SetCurState("wait");
    
    this->AddInputPort("detect_in");
    this->AddOutputPort("detect_out");

    logger_world    << "["
                    << myName
                    << "]"
                    <<" Created"
                    <<std::endl;

    logger_system   << "[" << "Detection::"
                    << myName
                    << ":Init]"
                    <<" Created, ID : "<<this->GetModelID()
                    <<", current State : "<<this->GetCurState()
                    <<std::endl;
}

bool Detection::ExtTransFn(const std::string& inPort, const std::any& message) {
    if(this->GetCurState()=="wait"){

        try {
            // msg = std::any_cast< MESSAGE TYPE >(message);
        } catch (const std::bad_any_cast&) {
            logger_system << "[ERROR] Invalid message type!" << std::endl;
            return false;
        }

    }else if(this->GetCurState()=="detect"){

    }
    return true;
}


bool Detection::IntTransFn() {
    return true;
}
bool Detection::OutputFn() {
    return true;
}
TIME_T Detection::TimeAdvanceFn() {
    if(this->GetCurState()=="wait")
        return TIME_INF;
    else if(this->GetCurState()=="detect")
        return 0.1;
    return -1;
}


// logging only
void Detection::UpdateTime(const TIME_T currentTime){
    this->lastTime = currentTime;
    this->nextTime = currentTime + TimeAdvanceFn();
    logger_system   << "[" << "Detection::"
                    << myName
                    << "::UpdateTime]"
                    << " LastTIme, NextTime = ("<<this->lastTime<<","<<this->nextTime<<")"
                    << std::endl;
}
void Detection::ReceiveScheduleTime(const TIME_T currentTime) {
    AtomicModel::ReceiveScheduleTime(currentTime);
    logger_system   << "[" << "Detection::"
                    << myName
                    << "::ReceiveScheduleTime]"
                    <<" Received (*,"<<currentTime<<"), next TA updated to "<<this->nextTime
                    <<std::endl;
}
void Detection::ReceiveEvent(Event& event,TIME_T currentTime) {
    AtomicModel::ReceiveEvent(event, currentTime);
    logger_system   << "[" << "Detection::"
                    << myName
                    << "::ReceiveEvent]"
                    <<" Received (x,"<<currentTime<<"), next TA updated to "<<this->nextTime
                    << std::endl;
}
const TIME_T Detection::QueryNextTime() const {
    logger_system   << "[" << "Detection::"
                    << myName
                    << "::QueryNextTime]"
                    <<" sends TA : "<<this->nextTime
                    << std::endl;
    return AtomicModel::QueryNextTime();
}