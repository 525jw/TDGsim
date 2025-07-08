#include "sim_maneuver.hpp"

Maneuver::Maneuver(int modelID, Engine* engine, std::string name)
    : AtomicModel(modelID, engine)
{
    this->myName = name;

    this->AddState("wait");
    this->AddState("move");

    this->SetCurState("wait");

    this->AddInputPort("m_in"); // ?MOVE_POS_INFO
    this->AddInputPort("stop_in"); // ?M_FLAG
    this->AddOutputPort("m_out"); // !CUR_POS
    this->AddOutputPort("m_rep"); // !CUR_POS
    
}

bool Maneuver::ExtTransFn(const std::string& inPort, const std::any& message) 
{

    if (inPort == "m_in")
    {
        MOVE_POS_INFO msg;
        try
        {
            msg = std::any_cast<MOVE_POS_INFO>(message);
        }
        catch(const std::bad_any_cast&)
        {
            logger_system << "[ERROR] Invalid message type!" << std::endl;
            return false;
        }

        if (this->GetCurState() == "wait") 
        {
            // 대기 상태에서 이동 명령을 받았을 때
            this->SetCurState("move");
            // 나중에 경로탐색 이쪽을 함수로 빼서 로직을 추가하건 해야할듯
            curPos = msg.curPos; // 현재 위치 좌표 설정
            detPos = msg.detPos; // 목적지 좌표 설정
            direction = (detPos.first > curPos.first) ? 1 : 0; // 방향 설정, 매우 단순한 방법
            isMoving = true; // 이동 중으로 설정
            return true;
        }
    }
    else if (inPort == "stop_in")
    {
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
        if (msg.isStop) 
        {   
        isMoving = false;
        this->SetCurState("wait");
        return true; // 이동 중지
        }
    }
    else return false;
}

bool Maneuver::IntTransFn() 
{
    if (this->GetCurState() == "move" && isMoving) 
    {
        // 이동 중인 상태에서 위치 업데이트
        if (curPos != detPos)
        {
            switch (direction)
            {
            case 0: // 왼쪽, x좌표 감소
                curPos.first -= moveSpeed;
                break;
            case 1: // 오른쪽, x좌표 증가
                curPos.first += moveSpeed;
                break;
            default:
                break;
            }
            if (curPos == detPos) 
            {
                // 목적지에 도달했을 때
                isMoving = false; // 이동 중지
                this->SetCurState("wait"); // 대기 상태로 전환
            }
        }
    }
    return true;
}

bool Maneuver::OutputFn() 
{
    if (this->GetCurState() == "move") 
    {
        // 이동 중인 상태에서 현재 위치를 출력
        CUR_POS pos;
        pos.pos = curPos;

        std::any msg = pos;

        this->AddOutputEvent("m_out", msg); // 위치 정보를 m_out 포트로 전송
        this->AddOutputEvent("m_rep", msg); // 위치 정보를 m_rep 포트로 전송
        return true;
    }
    return false; // 대기 상태에서는 출력하지 않음
}

TIME_T Maneuver::TimeAdvanceFn() 
{
    if (this->GetCurState() == "wait") 
    {
        return TIME_INF; // 대기 상태에서는 무한 대기
    }
    else if (this->GetCurState() == "move") 
    {
        return 1.0f; // 이동 중인 상태에서는 1초 후에 다음 이벤트 발생
    }
    return TIME_INF; // 그 외의 경우 무한 대기
}

void Maneuver::UpdateTime(const TIME_T currentTime)
{
    this->lastTime = currentTime;
    this->nextTime = currentTime + TimeAdvanceFn();
}

void Maneuver::ReceiveScheduleTime(const TIME_T currentTime)
{
    AtomicModel::ReceiveScheduleTime(currentTime);
}

void Maneuver::ReceiveEvent(Event& event,TIME_T currentTime)
{
    AtomicModel::ReceiveEvent(event, currentTime);
}

const TIME_T Maneuver::QueryNextTime() const
{
    return AtomicModel::QueryNextTime();
}