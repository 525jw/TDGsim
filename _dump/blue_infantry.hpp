#pragma once
#include "DEVS/coupled_model.hpp"
#include "DEVS/logger.hpp"
#include "common.hpp"
#include "PlatoonLeader/platoon_leader.hpp"
#include "Soldier/soldier.hpp"

class Infantry : public CoupledModel{
public:
    Infantry(Engine* engine)
    : CoupledModel(engine)
    {
        // Soldiers
        std::vector<int> soldiersId_PL1 = {1,2,3};
        std::vector<BlueSoldier*> soldiers_PL1;
        int numOfPl1 = soldiersId_PL1.size();
        soldiers_PL1.reserve(numOfPl1);
        soldiers_PL1[0] = new BlueSoldier(engine,1,Entity{Side::BLUE, ForceType::RIFLE, Point{3, 14}, "BLUE-PL1-SOL1"});
        soldiers_PL1[1] = new BlueSoldier(engine,2,Entity{Side::BLUE, ForceType::RIFLE, Point{5, 15}, "BLUE-PL1-SOL2"});
        soldiers_PL1[2] = new BlueSoldier(engine,3,Entity{Side::BLUE, ForceType::RIFLE, Point{4, 13}, "BLUE-PL1-SOL3"});
        for(int i=0;i<numOfPl1;i++){
            this->engine->RegisterModelInEngine(soldiers_PL1[i]);
            this->RegisterSubModel(soldiers_PL1[i]);
            soldiers_PL1[i]->SetParentModel(this);
        }
        // PlatoonLeader
        PlatoonLeader* leader_PL1 = new PlatoonLeader(engine,11,&soldiersId_PL1);
        this->engine->RegisterModelInEngine(leader_PL1);
        this->RegisterSubModel(leader_PL1);
        leader_PL1->SetParentModel(this);

        this->AddInputPort("CompanyOrd");
        this->AddOutputPort("InfantryRep");
        
        this->AddInputPort("FireIn");
        this->AddInputPort("PositionIn");
        this->AddOutputPort("FireOut");
        this->AddOutputPort("PositionOut");

        this->AddCoupling(this,"CompanyOrd",leader_PL1,"CompanyOrd",EIC);
        this->AddCoupling(leader_PL1,"PlatoonRep",this,"InfantryRep",EOC);

        for(int i=0;i<numOfPl1;i++){
            this->AddCoupling(leader_PL1,"PlatoonOrd",soldiers_PL1[i],"PlatoonOrd",IC);
            this->AddCoupling(soldiers_PL1[i],"SoldierRep",leader_PL1,"SoldeirRep",IC);

            this->AddCoupling(this,"FireIn",soldiers_PL1[i],"FireIn",EIC);
            this->AddCoupling(this,"PositionIn",soldiers_PL1[i],"PositionIn",EIC);
            this->AddCoupling(soldiers_PL1[i],"FireOut",this,"FireOut",EOC);
            this->AddCoupling(soldiers_PL1[i],"PositionOut",this,"PositionOut",EOC);
        }
    }
};