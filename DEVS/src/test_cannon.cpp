#pragma once
#include "atomic_model.hpp"
#include "test_world.hpp"
#include "message.cpp"
#include "logger.hpp"
#include <utility>
#include <iostream>
#include <string>

class TestCannon : public AtomicModel{
private:
    std::pair<int, int> myPosXY;
    int health = 100;
    std::string myName;
public:
    TestCannon(int modelID, Engine* engine, std::string name)
        :  AtomicModel(modelID, engine)
    {
        this->myName=name;
        std::pair<int,int> myPosXY;
        int x,y;
        do {
            x = std::rand() % 5;
            y = std::rand() % 5;
        } while (worldMap[y][x] != 0);
        myPosXY.first=x; myPosXY.second=y;
        worldMap[y][x]=1;
        logger<<"! ! ! "<<myName<<" created id:"<<this->GetModelID()<<", pos(x,y) = ("<<myPosXY.first<<","<<myPosXY.second<<") \n";

        this->AddState("idle");
        this->AddState("engage");
        this->AddState("dead");

        this->SetCurState("idle");
        
        this->AddInputPort("fire_in");
        this->AddOutputPort("fire_out");
    }

    bool ExtTransFn(const std::string& inPort, const FIREINFO& message) {
        if(inPort == "fire_in" && this->GetCurState() != "dead"){
            if(this->myPosXY.first == message.targetX && this->myPosXY.second == message.targetY){
                health -= message.damage;
                logger<<"[Model] "<<this->myName<<"hit ! health = "<<this->health<<std::endl;
            }else
                return true;
            if(health > 0){
                this->SetCurState("engage");
            }else{
                this->SetCurState("dead");
                worldMap[this->myPosXY.second][this->myPosXY.first]=0;
            }
        }
        return true;
    }
    bool IntTransFn() {
        if(this->GetCurState()=="idle"){
            this->SetCurState("engage");
        }else if(this->GetCurState()=="engage"){
            // TODO : transition engage -> dead when health is low
        }
        return true;
    }
    bool OutputFn() {
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
            std::any msg = info;

            this->AddOutputEvent("fire_out",msg);
        }
        return true;
    }
    TIME_T TimeAdvanceFn() {
        if (this->GetCurState() == "engage") {
            int steps = 10; // 0.5<= N <1.5 는 0.1 간격으로 10개 값 (0.5, 0.6, ..., 1.4)
            int rnd = std::rand() % steps; // 0 ~ 9
            return 0.5f + rnd * 0.1f;
        }
        return -1;
    }    
    void UpdateTime(const TIME_T engineTime){
        this->lastTime = engineTime;
        this->nextTime = engineTime + TimeAdvanceFn();
        logger<<"! ! ! "<<this->myName<<" : lastTIme, nextTime = ("<<this->lastTime<<","<<this->nextTime<<") \n";
    }
    void ReceiveScheduleTime(const TIME_T engineTime) override{
        AtomicModel::ReceiveScheduleTime(engineTime);
        logger<<"! ! ! "<<this->myName<<" received (*,"<<engineTime<<"), next TA updated to "<<this->nextTime<<"\n";
    }
    void ReceiveEvent(Event& externalEvent,TIME_T engineTime) override{
        AtomicModel::ReceiveEvent(externalEvent, engineTime);
        logger<<"! ! ! "<<this->myName<<" received (x,"<<engineTime<<"), next TA updated to "<<this->nextTime<<"\n";
    }
    const TIME_T QueryNextTime() const override{
        logger<<"! ! ! "<<this->myName<<" ("<<this->GetCurState()<<") "<<" sends TA : "<<this->nextTime<<std::endl;
        return AtomicModel::QueryNextTime();
    }
};