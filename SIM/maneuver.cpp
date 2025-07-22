#include "maneuver.hpp"
#include <cmath>

Maneuver::Maneuver(int modelID, Engine* engine, int objectID)
    : AtomicModel(modelID, engine)
{
    this->objectID = objectID;

    this->AddState("WAIT");
    this->AddState("MOVE");

    this->SetCurState("WAIT");

    this->AddInputPort("mnv_ord");
    this->AddInputPort("dead_in");
    this->AddOutputPort("mnv_result");
}

TIME_T mnvEquation(float speed) {
    return 1.0f / speed;
}

bool Maneuver::ExtTransFn(const std::string& inPort, const std::any& message) {
    if (inPort == "mnv_ord") {
        try {
            const MnvOrd msg = std::any_cast<MnvOrd>(message);

            if (msg.speed <= 0.0f) {
                this->SetCurState("WAIT");
                return true;
            }else{
                this->SetCurState("MOVE");
            }

            this->objectID = this->objectID;
            this->cur_pos = msg.next_pos;
            this->cur_speed = msg.speed;

        } catch (const std::bad_any_cast&) {
            logger_system << "[ERROR] Invalid message! port name : mnv_ord" << std::endl;
            return false;
        }
    } else if (inPort == "dead_in") {
        this->SetCurState("WAIT");
    }

    return true;
}

bool Maneuver::IntTransFn() {
    if (this->GetCurState() == "MOVE") {
        this->t_mnv = mnvEquation(this->cur_speed);
        // this->SetCurState("MOVE"); 이론적
    }
    return true;
}

bool Maneuver::OutputFn() {
    if (this->GetCurState() == "MOVE") {
        MnvResult msg_raw;
        msg_raw.objectID = this->objectID;
        msg_raw.cur_pos = this->cur_pos;
        std::any msg_final = msg_raw;
        this->AddOutputEvent("mnv_result", msg_final);
    }
    return true;
}

TIME_T Maneuver::TimeAdvanceFn() {
    if (this->GetCurState() == "WAIT") return TIME_INF;
    if (this->GetCurState() == "MOVE") return t_mnv;
    return -1;
}

void Maneuver::UpdateTime(const TIME_T currentTime){
    this->lastTime = currentTime;
    this->nextTime = currentTime + TimeAdvanceFn();
}

void Maneuver::ReceiveScheduleTime(const TIME_T currentTime) {
    AtomicModel::ReceiveScheduleTime(currentTime);
}

void Maneuver::ReceiveEvent(Event& event, TIME_T currentTime) {
    AtomicModel::ReceiveEvent(event, currentTime);
}

const TIME_T Maneuver::QueryNextTime() const {
    return AtomicModel::QueryNextTime();
}
