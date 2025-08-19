#pragma once
#include "DEVS/coupled_model.hpp"
#include "DEVS/logger.hpp"
#include "platoon_leader.hpp"
#include "blue_soldier.hpp"
#include "platoon_leader.hpp"

#include <string>
#include <vector>

class BlueInfantry : public CoupledModel{
public:
    BlueInfantry(Engine* engine)
    : CoupledModel(engine)
    {
        // entity Id hard coded;
        int entityId_ICoy7Pl=100; // I중대7소대
        int entityId_ICoy8Pl=200; // I중대8소대

        PlatoonLeader* pl7Leader = new PlatoonLeader(engine, entityId_ICoy7Pl, "ICoy_7Pl");
        this->RegisterModelWithID(pl7Leader);
        pl7Leader->SetParentModel(this);

        PlatoonLeader* pl8Leader = new PlatoonLeader(engine, entityId_ICoy8Pl, "ICoy_8Pl");
        this->RegisterModelWithID(pl8Leader);
        pl8Leader->SetParentModel(this);


        std::vector<BlueSoldier*> pl7Soldiers;
        std::vector<BlueSoldier*> pl8Soldiers;

        pl7Soldiers.reserve(25);
        pl8Soldiers.reserve(25);

        for(int i=1;i<=25;i++){
            std::string name7 = std::string("ICoy_7Pl_") + std::to_string(i);
            std::string name8 = std::string("ICoy_8Pl_") + std::to_string(i);

            auto* s7 = new BlueSoldier(engine, entityId_ICoy7Pl + i, name7);
            auto* s8 = new BlueSoldier(engine, entityId_ICoy8Pl + i, name8);

            this->RegisterModelWithID(s7);
            this->RegisterModelWithID(s8);
            s7->SetParentModel(this);
            s8->SetParentModel(this);
            
            pl7Soldiers.push_back(s7);
            pl8Soldiers.push_back(s8);
        }   

        this->AddInputPort("start");
        this->AddOutputPort("result");

        this->AddCoupling(this,"start",pl7Leader,"ScenInfo",EIC);
    }
};