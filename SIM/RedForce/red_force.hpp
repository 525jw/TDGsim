#pragma once
#include "DEVS/coupled_model.hpp"
#include "message.hpp"
#include "DEVS/logger.hpp"
#include "SIM/Environment/environment.hpp"
#include "red_soldier.hpp"

class RedForce : public CoupledModel{
public:
    RedForce(Engine* engine)
    : CoupledModel(engine)
    {
        // 생성
        std::vector<int> soldiersId_PL1 = {1,2,3};
        std::vector<RedSoldier*> soldiers_PL1;
        int numOfPl1 = soldiersId_PL1.size();
        soldiers_PL1.reserve(numOfPl1);
        soldiers_PL1[0] = new RedSoldier(engine,101,Entity{Team::RED, ForceType::RIFLE, Point{9, 10}, "RED-PL1-SOL1"});
        soldiers_PL1[1] = new RedSoldier(engine,102,Entity{Team::RED, ForceType::RIFLE, Point{23, 3}, "RED-PL1-SOL2"});
        soldiers_PL1[2] = new RedSoldier(engine,103,Entity{Team::RED, ForceType::RIFLE, Point{24, 2}, "RED-PL1-SOL3"});
        for(int i=0;i<numOfPl1;i++){
            this->engine->RegisterModelInEngine(soldiers_PL1[i]);   // engine에 등록
            this->RegisterSubModel(soldiers_PL1[i]);                // 자식모델 목록에 등록
            soldiers_PL1[i]->SetParentModel(this);                  // 부모를 this로 설정
        }
        // IO port 설정
        this->AddInputPort("BluePosition");
        this->AddInputPort("BlueFire");
        this->AddOutputPort("RedPosition");
        this->AddOutputPort("RedFire");
        for(int i=0;i<numOfPl1;i++){
            this->AddCoupling(this,"BluePosition",soldiers_PL1[i],"PositionIn",EIC);
            this->AddCoupling(this,"BlueFire",soldiers_PL1[i],"FireIn",EIC);
            this->AddCoupling(soldiers_PL1[i],"PositionOut",this,"RedPosition",EOC);
            this->AddCoupling(soldiers_PL1[i],"FireOut",this,"RedFire",EOC);
        }
    }
};