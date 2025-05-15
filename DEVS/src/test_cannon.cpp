#pragma once
#include "atomic_model.hpp"
#include "test_world.hpp"
#include "message.cpp"
#include <utility>
#include <iostream>
#include <string>

class TestCannon : public AtomicModel{
private:
    std::pair<int, int> myPosXY;
    int health = 100;
    std::string myName;
public:
    TestCannon(Engine* engine, std::string name)
        :  AtomicModel(engine)
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
        std::cout<<"! ! ! "<<myName<<" created , pos(x,y) = ("<<myPosXY.first<<","<<myPosXY.second<<") \n";

        this->AddState("engage");
        this->AddState("dead");

        this->SetCurState("engage");
        
        this->AddInputPort("fire_in");
        this->AddOutputPort("fire_out");
    }

    bool ExtTransFn(const std::string& inPort, const FIREINFO& message) {
        if(inPort == "fire_in" && this->GetCurState() != "dead"){
            if(this->myPosXY.first == message.targetX && this->myPosXY.second == message.targetY){
                health -= message.damage;
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
        if(this->GetCurState()=="engage"){
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
        if(this->GetCurState()=="engage"){
            return 0.5f + static_cast<TIME_T>(rand()) / RAND_MAX;
        }
        return -1;
    }
    void UpdateTime(const TIME_T engineTime){
        this->lastTime = engineTime;
        this->nextTime = engineTime + TimeAdvanceFn();
        std::cout<<this->myName<<" : lastTIme, nextTime = ("<<this->lastTime<<","<<this->nextTime<<") \n";
    }
};