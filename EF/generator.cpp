#include "generator.hpp"

Generator::Generator(int modelID, Engine* engine, std::string name)
    :  AtomicModel(modelID, engine)
{
    this->myName=name;

    this->AddState("Start");
    this->AddState("Wait");

    this->SetCurState("Start");
    
    this->AddInputPort("restart");
    this->AddOutputPort("start");

    logger_world    << "["
                    << myName
                    << "]"
                    <<" Created"
                    <<std::endl;

    logger_system   << "[" << "Generator::"
                    << myName
                    << ":Init]"
                    <<" Created, ID : "<<this->GetModelID()
                    <<", current State : "<<this->GetCurState()
                    <<std::endl;
}

bool Generator::ExtTransFn(const std::string& inPort, const std::any& message) {
    if(inPort == "restart" && this->GetCurState() != "Wait"){
        START_MSG msg;

        try {
            msg = std::any_cast<START_MSG>(message);
        } catch (const std::bad_any_cast&) {
            logger_system << "[ERROR] Invalid message type!" << std::endl;
            return false;
        }

        this->SetCurState("START");

        logger_system   << "[" << "Generator::"
                        << myName
                        << "::ExtTransFn]"
                        << " CurState : "<<this->GetCurState()
                        <<std::endl;
    }
    return true;
}

bool Generator::IntTransFn() {
    if(this->GetCurState()=="Start"){
        this->SetCurState("dead");
    }
    return true;
}

bool Generator::OutputFn() {
    if (this->GetCurState() == "Start") {
        START_MSG start_msg;
        // sim에 전달할 초기값 생성/저장
        // ex) 시드값, 시나리오 매개변수(맵, 병력구성 등)
        std::any msg = start_msg;

        this->AddOutputEvent("start",msg);
    }
    return true;
}
TIME_T Generator::TimeAdvanceFn() {
    if (this->GetCurState() == "Start") {
        return 0.0f;
    }else if(this->GetCurState()=="Wait"){
        return TIME_INF;
    }
    return -1;
}    
void Generator::UpdateTime(const TIME_T currentTime){
    this->lastTime = currentTime;
    this->nextTime = currentTime + TimeAdvanceFn();
    logger_system   << "[" << "Generator::"
                    << myName
                    << "::UpdateTime]"
                    << " LastTIme, NextTime = ("<<this->lastTime<<","<<this->nextTime<<")"
                    << std::endl;
}
void Generator::ReceiveScheduleTime(const TIME_T currentTime) {
    AtomicModel::ReceiveScheduleTime(currentTime);
    logger_system   << "[" << "Generator::"
                    << myName
                    << "::ReceiveScheduleTime]"
                    <<" Received (*,"<<currentTime<<"), next TA updated to "<<this->nextTime
                    <<std::endl;
}
void Generator::ReceiveEvent(Event& event,TIME_T currentTime) {
    AtomicModel::ReceiveEvent(event, currentTime);
    logger_system   << "[" << "Generator::"
                    << myName
                    << "::ReceiveEvent]"
                    <<" Received (x,"<<currentTime<<"), next TA updated to "<<this->nextTime
                    << std::endl;
}
const TIME_T Generator::QueryNextTime() const {
    logger_system   << "[" << "Generator::"
                    << myName
                    << "::QueryNextTime]"
                    <<" sends TA : "<<this->nextTime
                    << std::endl;
    return AtomicModel::QueryNextTime();
}