#include "sim_maneuver.hpp"

Maneuver::Maneuver(int modelID, Engine* engine, std::string name)
    : AtomicModel(modelID, engine)
{
    this->myName = name;
    int x, y;
    // 처음 생성할 때, 위치 정보를 어디서 가져오지?
    myPosXY.first = x; myPosXY.second = y;

    this->AddState("wait");
    this->AddState("move");

    this->SetCurState("wait");

    this->AddInputPort("m_in"); // m_in or m_ord, Packet: det_pos
    this->AddInputPort("stop_in"); // 정지! Packet: M_FLAG
    this->AddOutputPort("m_out"); // m_out or pos_out, Packet: myPosXY
    this->AddOutputPort("m_rep"); // DM으로 보내는 포트

    logger_world << "["
                 << myName
                 << "]"
                 << " Created, pos(x,y) = (" << myPosXY.first << "," << myPosXY.second << ")"
                 << std::endl;

    logger_system << "[" << "Maneuver::"
                  << myName
                  << ":Init]"
                  << " Created, ID : " << this->GetModelID()
                  << ", pos(x,y) = (" << myPosXY.first << "," << myPosXY.second << ")"
                  << ", current State : " << this->GetCurState()
                  << std::endl;
}

bool Maneuver::ExtTransFn(const std::string& inPort, const std::any& message) {
    // 메시지 전처리
    if (inPort == "m_in" && this->GetCurState() != "dead") {
        DET_POS msg;    // 이걸 massage.hpp에 정의해야함
        
        try
        {
            msg = std::any_cast<DET_POS>(message);
        }
        catch(const std::bad_any_cast&)
        {
            logger_system << "[ERROR] Invalid message type!" << std::endl;
            return false;
        }

        // 이동 상태로 처리
        this->SetCurState("move");
        detPos = msg.detPos; // 목적지 좌표 설정
        direction = (detPos.first > myPosXY.first) ? 1 : 0; // 방향 설정, 매우 단순한 방법
        isMoving = true; // 이동 중으로 설정
        
    }

    if (inPort == "stop_in") {
        M_FLAG msg;

        try
        {
            msg = std::any_cast<M_FLAG>(message);
        }
        catch(const std::bad_any_cast&)
        {
            logger_system << "[ERROR] Invalid message type!" << std::endl;
            return false;
        }

        // 이동 중지
        isMoving = false;
        this->SetCurState("wait");
        
        logger_world << "[" << myName << "]" 
                     << " Stopped at position (" << myPosXY.first << "," << myPosXY.second << ")"
                     << std::endl;

        logger_system << "[" << "Maneuver::" 
                      << myName 
                      << "::ExtTransFn]"
                      << " Stopped, Current Position: (" << myPosXY.first << "," << myPosXY.second << ")"
                      << std::endl;
    }

    return true;
}

bool Maneuver::IntTransFn() {
    if (this->GetCurState() == "move") {
        // 이동 중인 상태에서 위치 업데이트
        if (isMoving) {
            if (direction == 1) {
                myPosXY.first += moveSpeed; // x좌표 증가
            } else {
                myPosXY.first -= moveSpeed; // x좌표 감소
            }
        }
    }
    return true;
}
bool Maneuver::OutputFn() {
    if (this->GetCurState() == "move") {
        // 이동 중인 상태에서 현재 위치를 출력
        std::any msg = myPosXY; // 현재 위치 정보를 메시지로 변환

        logger_world << "[" << myName << "]"
                     << " Moving to position (" << myPosXY.first << "," << myPosXY.second << ")"
                     << std::endl;

        logger_system << "[" << "Maneuver::"
                      << myName
                      << "::OutputFn]"
                      << " Moving, Current Position: (" << myPosXY.first << "," << myPosXY.second << ")"
                      << std::endl;

        this->AddOutputEvent("m_out", msg); // 위치 정보를 m_out 포트로 전송
}

TIME_T Maneuver::TimeAdvanceFn() {
    if (isMoving) {
        return 1.0f;
    }
    return TIME_INF;
}

void Maneuver::UpdateTime(const TIME_T currentTime){
    this->lastTime = currentTime;
    this->nextTime = currentTime + TimeAdvanceFn();
    logger_system   << "[" << "Maneuver::"
                    << myName
                    << "::UpdateTime]"
                    << " LastTime, NextTime = ("<<this->lastTime<<","<<this->nextTime<<")"
                    << std::endl;
}

void Maneuver::ReceiveScheduleTime(const TIME_T currentTime){
    Atomic_Model::ReceiveScheduleTime(currentTime);
    logger_system   << "[" << "Maneuver::"
                    << myName
                    << "::ReceiveScheduleTime]"
                    <<" Received (*,"<<currentTime<<"), next TA updated to "<<this->nextTime
                    <<std::endl;
}
void Maneuver::ReceiveEvent(Event& event,TIME_T currentTime){
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
