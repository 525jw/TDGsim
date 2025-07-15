#include "maneuver.hpp"

Maneuver::Maneuver(int modelID, Engine* engine, std::string name)
    :  AtomicModel(modelID, engine)
{
    this->myName=name;

    this->AddState("Wait");
    this->AddState("Move");

    this->SetCurState("Wait");
    
    this->AddInputPort("mnv_in..../////");
    this->AddOutputPort("");

    logger_world    << "["
                    << myName
                    << "]"
                    <<" Created"
                    <<std::endl;

    logger_system   << "[" << "Maneuver::"
                    << myName
                    << ":Init]"
                    <<" Created, ID : "<<this->GetModelID()
                    <<", current State : "<<this->GetCurState()
                    <<std::endl;
}

bool Maneuver::ExtTransFn(const std::string& inPort, const std::any& message) {
    if(inPort == "result" && this->GetCurState() != "Wait"){
        RESULT_MSG msg;

        try {
            msg = std::any_cast<RESULT_MSG>(message);
        } catch (const std::bad_any_cast&) {
            logger_system << "[ERROR] Invalid message type!" << std::endl;
            return false;
        }

        this->SetCurState("Restart");

        logger_system   << "[" << "Maneuver::"
                        << myName
                        << "::ExtTransFn]"
                        << " CurState : "<<this->GetCurState()
                        <<std::endl;
    }
    return true;
}

bool Maneuver::IntTransFn() {
    if(this->GetCurState()=="Restart"){
        this->SetCurState("Wait");
    }
    return true;
}
bool Maneuver::OutputFn() {
    if (this->GetCurState() == "Restart") {
        START_MSG start_msg;
        // RESTART_MSG restart_msg
        // payload 다르다면 message 새로 추가 ex) 같은 맵 다른 시드 실행
        // sim에 전달할 값 생성/저장
        // ex) 시드값, 시나리오 매개변수(맵, 병력구성 등)
        std::any msg = start_msg;

        this->AddOutputEvent("restart",msg);
    }
    return true;
}
TIME_T Maneuver::TimeAdvanceFn() {
    if (this->GetCurState() == "Restart") {
        return 0.0f;
    }else if(this->GetCurState()=="Wait"){
        return TIME_INF;
    }
    return -1;
}    
void Maneuver::UpdateTime(const TIME_T currentTime){
    this->lastTime = currentTime;
    this->nextTime = currentTime + TimeAdvanceFn();
    logger_system   << "[" << "Maneuver::"
                    << myName
                    << "::UpdateTime]"
                    << " LastTIme, NextTime = ("<<this->lastTime<<","<<this->nextTime<<")"
                    << std::endl;
}
void Maneuver::ReceiveScheduleTime(const TIME_T currentTime) {
    AtomicModel::ReceiveScheduleTime(currentTime);
    logger_system   << "[" << "Maneuver::"
                    << myName
                    << "::ReceiveScheduleTime]"
                    <<" Received (*,"<<currentTime<<"), next TA updated to "<<this->nextTime
                    <<std::endl;
}
void Maneuver::ReceiveEvent(Event& event,TIME_T currentTime) {
    AtomicModel::ReceiveEvent(event, currentTime);
    logger_system   << "[" << "Maneuver::"
                    << myName
                    << "::ReceiveEvent]"
                    <<" Received (x,"<<currentTime<<"), next TA updated to "<<this->nextTime
                    << std::endl;
}
const TIME_T Maneuver::QueryNextTime() const {
    logger_system   << "[" << "Maneuver::"
                    << myName
                    << "::QueryNextTime]"
                    <<" sends TA : "<<this->nextTime
                    << std::endl;
    return AtomicModel::QueryNextTime();
}