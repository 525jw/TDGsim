#include "sim_detection.hpp"

Detection::Detection(int modelID, Engine* engine, std::string name)
    : AtomicModel(modelID, engine)
{
    this->myName = name;

    this->AddState("idle");
    this->AddState("process");
    this->AddState("report");

    this->SetCurState("idle");

    this->AddInputPort("d_in"); // d_in or d_ord, Packet: curPos, tgtPos
    this->AddInputPort("stop_in"); // 정지! Packet: M_FLAG
    this->AddOutputPort("d_out"); // d_out, Packet: tgtPos, isDetect
    this->AddOutputPort("d_rep"); // DM으로 보내는 포트
}

bool Detection::ExtTransFn(const std::string& inPort, const std::any& message) 
{

    if (inPort == "d_in")
    {
        DETECT_REQ_INFO msg;
        try
        {
            msg = std::any_cast<DETECT_REQ_INFO>(message);
        }
        catch(const std::bad_any_cast&)
        {
            logger_system << "[ERROR] Invalid message type!" << std::endl;
            return false;
        }

        if (this->GetCurState() == "idle") 
        {
            // 대기 상태에서 탐지 처리 요청을 받았을 때
            this->SetCurState("process");
            isDetecting = true; // 탐지 중으로 설정
            curPos = msg.curPos; // 현재 위치 좌표 설정
            tgtPos = msg.tgtPos; // 표적 위치 좌표 설정
            // 탐지 로직: 현재 위치와 표적 위치의 맨해튼 거리를 계산
            actualRange = std::abs(curPos.first - tgtPos.first) + std::abs(curPos.second - tgtPos.second);
            if (actualRange <= viewRange) 
            {
                isDectected = true;
            }
            else 
            {
                isDectected = false;
            }
            return true;
        }
        else return false;  // 대기 상태가 아니면 요청 처리 불가
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
        isDetecting = false;
        this->SetCurState("idle");
        return true;
        }
    }
    return false;
}

bool Detection::IntTransFn() 
{
    if (this->GetCurState() == "process" && isDetecting) 
    {
        // 탐지 중인 상태에서 위치 업데이트
        // 현재 위치와 표적 위치의 맨해튼 거리를 계산
        actualRange = std::abs(curPos.first - tgtPos.first) + std::abs(curPos.second - tgtPos.second);
        if (actualRange <= viewRange) 
        {
            isDectected = true;
        }
        else 
        {
            isDectected = false;
        }
        this->SetCurState("report"); 
    }
    else if (this->GetCurState() == "report")
    {
        this->SetCurState("idle"); // 보고 후 대기 상태로 전환
    }   
    return true;
}

bool Detection::OutputFn() 
{
    if (this->GetCurState() == "report") 
    {
        DETECT_RTN_INFO info;
        info.tgtPos = tgtPos;
        info.isDetect = isDectected;

        std::any msg = info;

        this->AddOutputEvent("d_out", msg); // 위치 정보를 d_out 포트로 전송
        this->AddOutputEvent("d_rep", msg); // 위치 정보를 d_rep 포트로 전송
        return true;
    }
    return false; // 대기 상태에서는 출력하지 않음
}

TIME_T Detection::TimeAdvanceFn() 
{
    if (this->GetCurState() == "idle") 
    {
        return TIME_INF; // 대기 상태에서는 무한 대기
    }
    else if (this->GetCurState() == "process") 
    {
        return 0.5f;
    }
    else if (this->GetCurState() == "report") 
    {
        return 0.1f; // 보고 상태에서는 짧은 시간
    }
    return TIME_INF;
}

void Detection::UpdateTime(const TIME_T currentTime)
{
    this->lastTime = currentTime;
    this->nextTime = currentTime + TimeAdvanceFn();
}

void Detection::ReceiveScheduleTime(const TIME_T currentTime)
{
    AtomicModel::ReceiveScheduleTime(currentTime);
}

void Detection::ReceiveEvent(Event& event,TIME_T currentTime)
{
    AtomicModel::ReceiveEvent(event, currentTime);
}

const TIME_T Detection::QueryNextTime() const
{
    return AtomicModel::QueryNextTime();
}