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

        // Process the valid message
        this->SetCurState("move");
        detPos = msg.detPos; // 목적지 좌표 설정
        direction = (detPos.first > myPosXY.first) ? 1 : 0; // 방향 설정, 매우 단순한 방법
        isMoving = true; // 이동 중으로 설정
        
    }

    return true;
}
