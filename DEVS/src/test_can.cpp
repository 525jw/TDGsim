#include "test_can.hpp"

TestCannon::TestCannon(int modelID, Engine* engine, std::string name)
    :  AtomicModel(modelID, engine)
{
    this->myName=name;
    int x,y;
    do {
        x = std::rand() % 5;
        y = std::rand() % 5;
    } while (worldMap[y][x] != 0);
    myPosXY.first=x; myPosXY.second=y;
    worldMap[y][x]=1;

    this->AddState("idle");
    this->AddState("engage");
    this->AddState("dead");

    this->SetCurState("idle");
    
    this->AddInputPort("fire_in");
    this->AddOutputPort("fire_out");

    logger_world    << "["
                    << myName
                    << "]"
                    <<" Created, pos(x,y) = ("<<myPosXY.first<<","<<myPosXY.second<<")"
                    <<std::endl;

    logger_system   << "[" << "TestCannon::"
                    << myName
                    << ":Init]"
                    <<" Created, ID : "<<this->GetModelID()
                    <<", pos(x,y) = ("<<myPosXY.first<<","<<myPosXY.second<<")"
                    <<", current State : "<<this->GetCurState()
                    <<std::endl;
}

bool TestCannon::ExtTransFn(const std::string& inPort, const std::any& message) {
    if(inPort == "fire_in" && this->GetCurState() != "dead"){
        FIREINFO msg;

        try {
            msg = std::any_cast<FIREINFO>(message);
        } catch (const std::bad_any_cast&) {
            logger_system << "[ERROR] Invalid message type!" << std::endl;
            return false;
        }

        if(this->myPosXY.first == msg.targetX && this->myPosXY.second == msg.targetY){
            health -= msg.damage;

            logger_world    << "["
                            << myName
                            << "]"
                            << " My location: ("<<myPosXY.first<<","<<myPosXY.second<<")"
                            << " Impact location: ("<<msg.targetX<<","<<msg.targetY<<")"
                            << " Damaged , health = "<<this->health
                            << " Sender : "<<msg.orgSenderModelID // DEBUG ONLY
                            <<std::endl;


            logger_system   << "[" << "TestCannon::"
                            << myName
                            << "::ExtTransFn]"
                            << " My location: ("<<myPosXY.first<<","<<myPosXY.second<<")"
                            << " Impact location: ("<<msg.targetX<<","<<msg.targetY<<")"
                            << " Damaged , health = "<<this->health
                            << " Sender : "<<msg.orgSenderModelID // DEBUG ONLY
                            <<std::endl;
        }else
            return true;
        if(health > 0){
            this->SetCurState("engage");
        }else{
            this->SetCurState("dead");
            worldMap[this->myPosXY.second][this->myPosXY.first]=0;
        }

        logger_system   << "[" << "TestCannon::"
                        << myName
                        << "::ExtTransFn]"
                        << " CurState : "<<this->GetCurState()
                        << " Health = "<<this->health
                        <<std::endl;

    }
    return true;
}
bool TestCannon::IntTransFn() {
    if(this->GetCurState()=="idle"){
        this->SetCurState("engage");
    }else if(this->GetCurState()=="engage"){
        // TODO : transition engage -> dead when health is low
    }
    return true;
}
bool TestCannon::OutputFn() {
    if (this->GetCurState() == "engage") {
        std::string outPort = "fire_info";

        // 무작위 타겟 위치 및 피해량 설정
        int targetX = std::rand() % 5;
        int targetY = std::rand() % 5;
        int damage = (std::rand() % 20) + 10; // 10 ~ 29 사이의 피해량
        FIREINFO info;
        info.targetX = targetX;
        info.targetY = targetY;
        info.damage = damage;
        info.orgSenderModelID = this->GetModelID(); // DEBUG ONLY;
        std::any msg = info;

        logger_world    << "["
                        << myName
                        << "]"
                        << " Fire , pos(x,y) = ("<<info.targetX<<","<<info.targetY<<")"
                        <<std::endl;

        logger_system   << "[" << "TestCannon::"
                        << myName
                        << "::OutputFn]"
                        << " Fire , pos(x,y) = ("<<info.targetX<<","<<info.targetY<<")"
                        <<std::endl;

        this->AddOutputEvent("fire_out",msg);
    }
    return true;
}
TIME_T TestCannon::TimeAdvanceFn() {
    if (this->GetCurState() == "engage") {
        int steps = 10; // 0.5<= N <1.5 는 0.1 간격으로 10개 값 (0.5, 0.6, ..., 1.4)
        int rnd = std::rand() % steps; // 0 ~ 9
        TIME_T ret = 0.5f + rnd * 0.1f;

        logger_system   << "[" << "TestCannon::"
                        << myName
                        << "::TimeAdvanceFn]"
                        << " TA calculated : " << ret
                        <<std::endl;

        return ret;
    }
    return -1;
}    
void TestCannon::UpdateTime(const TIME_T currentTime){
    this->lastTime = currentTime;
    this->nextTime = currentTime + TimeAdvanceFn();
    logger_system   << "[" << "TestCannon::"
                    << myName
                    << "::UpdateTime]"
                    << " LastTIme, NextTime = ("<<this->lastTime<<","<<this->nextTime<<")"
                    << std::endl;
}
void TestCannon::ReceiveScheduleTime(const TIME_T currentTime) {
    AtomicModel::ReceiveScheduleTime(currentTime);
    logger_system   << "[" << "TestCannon::"
                    << myName
                    << "::ReceiveScheduleTime]"
                    <<" Received (*,"<<currentTime<<"), next TA updated to "<<this->nextTime
                    <<std::endl;
}
void TestCannon::ReceiveEvent(Event& event,TIME_T currentTime) {
    AtomicModel::ReceiveEvent(event, currentTime);
    logger_system   << "[" << "TestCannon::"
                    << myName
                    << "::ReceiveEvent]"
                    <<" Received (x,"<<currentTime<<"), next TA updated to "<<this->nextTime
                    << std::endl;
}
const TIME_T TestCannon::QueryNextTime() const {
    logger_system   << "[" << "TestCannon::"
                    << myName
                    << "::QueryNextTime]"
                    <<" sends TA : "<<this->nextTime
                    << std::endl;
    return AtomicModel::QueryNextTime();
}