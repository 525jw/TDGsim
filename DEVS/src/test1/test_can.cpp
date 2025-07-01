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
        FIRE_INFO msg;

        try {
            msg = std::any_cast<FIRE_INFO>(message);
        } catch (const std::bad_any_cast&) {
            logger_system << "[ERROR] Invalid message type!" << std::endl;
            return false;
        }

        hitCountMap[msg.targetX][msg.targetY]++;

        if(this->myPosXY.first == msg.targetX && this->myPosXY.second == msg.targetY){
            health -= msg.damage;
            damageCount ++;

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

    }else if(inPort == "log_ord"){
        LOG_ORD msg;

        try {
            msg = std::any_cast<LOG_ORD>(message);
        } catch (const std::bad_any_cast&) {
            logger_system << "[ERROR] Invalid message type!" << std::endl;
            return false;
        }

        logger_system   << "[" << "TestCannon::"
                        << myName
                        << "::ExtTransFn]"
                        << " logging status"
                        << " CurState : "<<this->GetCurState()
                        << " Health = "<<this->health
                        << " Damage Count = "<<this->damageCount
                        << " Fire Count = "<<this->fireCount
                        <<std::endl;

        logger_world    << "["
                        << myName
                        << "]"
                        << " State : "<<this->GetCurState()
                        << " location: ("<<myPosXY.first<<","<<myPosXY.second<<")"
                        << " Health = "<<this->health
                        << " Damage Count = "<<this->damageCount
                        << " Fire Count = "<<this->fireCount
                        <<std::endl;
    }
    return true;
}

/*
NOTE : 내부천이로는 *체력50이하일 때 1초를 넘기면 죽음*이 구현되지 않는다
    내부천이의 발생이 TA에 의존하고, TA는 오직 minTA를 건네준 모델에만 전달되기 때문(ref.교재 when receive (*,t))
    따라서 *체력이 변화되는 시점 : ExtTransFn*에서 *위독함 State*로 전이 후
    해당 State의 TimeAdvanceFn를 1초로 지정하는게 합당하다

    아래 코드는 *체력이 50이하 && 1초가 경과일 때 내가 어떤 행동이든 수행하면 사망*에 가깝다 
    
    >> 행위는 TA에서 수행
    >> 결과는 TransFn에서 관측
    >> 모델이 언제 했다고 우기면 엔진이 빠른 순으로 반영해주는 개념에 가까움
*/
bool TestCannon::IntTransFn() {
    if(this->GetCurState()=="idle"){
        this->SetCurState("engage");
    }else if(this->GetCurState()=="engage"){
        if(this->health <= 50 && (this->nextTime - this->lastTime) >= 1.0){
            this->SetCurState("dead");
            logger_world    << "["
                            << myName
                            << "]"
                            << " Died from heavy bleeding"
                            <<std::endl;

            logger_system   << "[" << "TestCannon::"
                            << myName
                            << "::IntTransFn]"
                            << " Died, Elapsed Time = "<<this->nextTime - this->lastTime
                            << " Health = "<<this->health
                            <<std::endl;
            this->health = 0;
            this->nextTime = TIME_INF;
            this->lastTime = TIME_INF;
        }
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
        
        FIRE_INFO info;
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
        fireCount ++;
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
    }else if(this->GetCurState()=="dead"){
        return TIME_INF;
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