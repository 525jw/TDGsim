#include "maneuver.hpp"
#include <cmath>

Maneuver::Maneuver(Engine* engine, int objectID)
    : AtomicModel(engine)
{
    this->objectID = objectID;

    this->AddState("WAIT");
    this->AddState("MOVE");

    this->SetCurState("WAIT");

    this->AddInputPort("mnvOrd");
    this->AddInputPort("deadIn");
    this->AddOutputPort("mnvRes");
}

TIME_T mnvEquation(float speed) {
    return 1.0f / speed;
}

bool Maneuver::ExtTransFn(const std::string& inPort, const std::any& anyMessage) {
    if (inPort == "mnvOrd" && this->GetCurState()=="WAIT") {
        MnvOrd message;
        if(!TryCastMessage(anyMessage,message,"")) return false;

        if (message.speed > 0.0f)
            this->SetCurState("MOVE");

        this->objectID = this->objectID;
        this->curPos = message.nextPos;
        this->curSpeed = message.speed;
    } else if (inPort == "deadIn") {
        this->SetCurState("WAIT");
    }
    return true;
}

bool Maneuver::IntTransFn() {
    if (this->GetCurState() == "MOVE") {
        this->t_mnv = mnvEquation(this->curSpeed);
        // this->SetCurState("MOVE"); 이론적
    }
    return true;
}

bool Maneuver::OutputFn() {
    if (this->GetCurState() == "MOVE") {
        MnvRes message;
        message.objectID = this->objectID;
        message.curPos = this->curPos;
        std::any anyMessage = message;
        this->AddOutputEvent("mnvRes", anyMessage);
    }
    return true;
}

TIME_T Maneuver::TimeAdvanceFn() {
    if (this->GetCurState() == "WAIT") return TIME_INF;
    if (this->GetCurState() == "MOVE") return t_mnv;
    return -1;
}