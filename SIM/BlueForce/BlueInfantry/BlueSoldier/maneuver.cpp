#include "maneuver.hpp"
#include <cmath>

Maneuver::Maneuver(Engine* engine, int entityId)
    : AtomicModel(engine)
{
    this->entityId = entityId;

    this->AddState("WAIT");
    this->AddState("MOVE");

    this->SetCurState("WAIT");

    this->AddInputPort("PlatoonOrd");
    this->AddOutputPort("MnvRes");
}

TIME_T mnvEquation(float speed) {
    return 1.0f / speed;
}

bool Maneuver::ExtTransFn(const std::string& inPort, const std::any& anyMessage) {
    if (inPort == "PlatoonOrd" && this->GetCurState()=="WAIT") {
        PlatoonOrd message;
        if(!TryCastMessage(anyMessage,message,"")) return false;

        this->entityId = this->entityId;
        this->curPos = message.detPos;
        this->curSpeed = 1.0f; // 현재 지형 읽고 값 적용
    }
    return true;
}

bool Maneuver::IntTransFn() {
    if (this->GetCurState() == "MOVE") {
        this->t_mnv = mnvEquation(this->curSpeed); // ?? 내부 상태변수 바꾸는게 IntFn or OutFn
    }
    return true;
}

bool Maneuver::OutputFn() {
    if (this->GetCurState() == "MOVE") {
        MnvRes message;
        message.senderId = this->entityId;
        message.curPos = this->curPos;
        std::any anyMessage = message;
        this->AddOutputEvent("MnvRes", anyMessage);
    }
    return true;
}

TIME_T Maneuver::TimeAdvanceFn() {
    if (this->GetCurState() == "WAIT") return TIME_INF;
    if (this->GetCurState() == "MOVE") return t_mnv;
    return -1;
}