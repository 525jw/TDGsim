#include "sim_decisionmaker.hpp"

DecisionMaker::DecisionMaker(int modelID, Engine* engine, std::string name)
    : AtomicModel(modelID, engine)
{
    this->myName = name;
    detPos = {0, 0};    // 이동 테스트용, 초기 목표 지점

    this->AddState("idle");
    this->AddState("mobilize");
    this->AddState("engage");
    this->AddState("dead");

    this->SetCurState("idle");

    this->AddOutputPort("m_out"); // !MOVE_POS_INFO
    this->AddOutputPort("stop_out"); // !M_FLAG
    this->AddInputPort("m_in"); // ?CUR_POS
    this->AddInputPort("m_rep"); // ?CUR_POS
}

bool DecisionMaker::ExtTransFn(const std::string& inPort, const std::any& message) 
{

    if (inPort == "m_in")
    {
        CUR_POS msg;
        try
        {
            msg = std::any_cast<CUR_POS>(message);
        }
        catch(const std::bad_any_cast&)
        {
            logger_system << "[ERROR] Invalid message type!" << std::endl;
            return false;
        }
        curPos = msg.pos;
        if (curPos == detPos) {
            this->SetCurState("idle"); // 목적지에 도달하면 대기 상태로 전환
        }
    }
    else if (inPort == "m_rep")
    {
        CUR_POS msg;
        try
        {
            msg = std::any_cast<CUR_POS>(message);
        }
        catch(const std::bad_any_cast&)
        {
            logger_system << "[ERROR] Invalid message type!" << std::endl;
            return false;
        }
        
    }
    else return false;
}

bool DecisionMaker::IntTransFn() 
{
    if (true)   // 아무튼 이동을 하고 싶음
    {
        this->SetCurState("mobilize"); // 이동 준비 상태로 전환
        return true;
    }
    return false;
}

bool DecisionMaker::OutputFn() 
{
    if (this->GetCurState() == "mobilize") 
    {
        // 이동 중인 상태에서 현재 위치를 출력
        MOVE_POS_INFO info;
        info.curPos = curPos; // 현재 위치 좌표
        info.detPos = detPos; // 목적지 좌표

        std::any msg = info;

        this->AddOutputEvent("m_out", msg); // 위치 정보를 m_out 포트로 전송
        this->AddOutputEvent("m_rep", msg); // 위치 정보를 m_rep 포트로 전송
        return true;
    }
    return false; // 대기 상태에서는 출력하지 않음
}

TIME_T DecisionMaker::TimeAdvanceFn() 
{
    if (this->GetCurState() == "idle") 
    {
        return TIME_INF; // 대기 상태에서는 무한 대기
    }
    else if (this->GetCurState() == "mobilize") 
    {
        return 1.0f; // 이동 중인 상태에서는 1초 후에 다음 이벤트 발생
    }
    else if (this->GetCurState() == "dead") 
    {
        return TIME_INF; // 사망 상태에서는 무한 대기
    }
    return TIME_INF; // 그 외의 경우 무한 대기
}

void DecisionMaker::UpdateTime(const TIME_T currentTime)
{
    this->lastTime = currentTime;
    this->nextTime = currentTime + TimeAdvanceFn();
}

void DecisionMaker::ReceiveScheduleTime(const TIME_T currentTime)
{
    AtomicModel::ReceiveScheduleTime(currentTime);
}

void DecisionMaker::ReceiveEvent(Event& event,TIME_T currentTime)
{
    AtomicModel::ReceiveEvent(event, currentTime);
}

const TIME_T DecisionMaker::QueryNextTime() const
{
    return AtomicModel::QueryNextTime();
}