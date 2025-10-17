#pragma once
#include "DEVS/coupled_model.hpp"
#include "common.hpp"
#include "DEVS/logger.hpp"
#include "SIM/Environment/environment.hpp"
#include "SIM/Infantry/Soldier/soldier.hpp"
#include "SIM/Infantry/PlatoonLeader/platoon_leader.hpp"
#include "SIM/HQ/hq.hpp"

class BlueForce : public CoupledModel{
public:
    BlueForce(Engine* engine)
    : CoupledModel(engine)
    {
        // INFANTRY
        // === 생성 ===
        std::vector<int> plt1_Ids; plt1_Ids.clear();
        std::vector<int> plt2_Ids; plt2_Ids.clear();
        std::vector<int> cmp_Ids; cmp_Ids.clear();

        std::vector<Soldier*> plt1;
        const int numOfPl1 = 25;
        plt1.reserve(numOfPl1);

        std::vector<Soldier*> plt2;
        const int numOfPl2 = 25;
        plt2.reserve(numOfPl2);

        // TODO: 나중에 하드코딩 고쳐놓기
        // === BLUE PLT1 ===
        plt1.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT1-SOL01"), "BLUE-PLT1-SOL01", Side::BLUE, ForceType::RIFLE, {14,68}})); plt1_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT1-SOL01"));
        plt1.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT1-SOL02"), "BLUE-PLT1-SOL02", Side::BLUE, ForceType::RIFLE, {13,67}})); plt1_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT1-SOL02"));
        plt1.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT1-SOL03"), "BLUE-PLT1-SOL03", Side::BLUE, ForceType::RIFLE, {13,68}})); plt1_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT1-SOL03"));
        plt1.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT1-SOL04"), "BLUE-PLT1-SOL04", Side::BLUE, ForceType::RIFLE, {13,69}})); plt1_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT1-SOL04"));
        plt1.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT1-SOL05"), "BLUE-PLT1-SOL05", Side::BLUE, ForceType::RIFLE, {14,67}})); plt1_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT1-SOL05"));
        plt1.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT1-SOL06"), "BLUE-PLT1-SOL06", Side::BLUE, ForceType::RIFLE, {14,69}})); plt1_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT1-SOL06"));
        plt1.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT1-SOL07"), "BLUE-PLT1-SOL07", Side::BLUE, ForceType::RIFLE, {15,67}})); plt1_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT1-SOL07"));
        plt1.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT1-SOL08"), "BLUE-PLT1-SOL08", Side::BLUE, ForceType::RIFLE, {15,68}})); plt1_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT1-SOL08"));
        plt1.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT1-SOL09"), "BLUE-PLT1-SOL09", Side::BLUE, ForceType::RIFLE, {15,69}})); plt1_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT1-SOL09"));
        plt1.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT1-SOL10"), "BLUE-PLT1-SOL10", Side::BLUE, ForceType::RIFLE, {12,66}})); plt1_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT1-SOL10"));
        plt1.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT1-SOL11"), "BLUE-PLT1-SOL11", Side::BLUE, ForceType::RIFLE, {12,67}})); plt1_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT1-SOL11"));
        plt1.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT1-SOL12"), "BLUE-PLT1-SOL12", Side::BLUE, ForceType::RIFLE, {12,68}})); plt1_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT1-SOL12"));
        plt1.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT1-SOL13"), "BLUE-PLT1-SOL13", Side::BLUE, ForceType::RIFLE, {13,66}})); plt1_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT1-SOL13"));
        plt1.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT1-SOL14"), "BLUE-PLT1-SOL14", Side::BLUE, ForceType::RIFLE, {14,66}})); plt1_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT1-SOL14"));
        plt1.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT1-SOL15"), "BLUE-PLT1-SOL15", Side::BLUE, ForceType::RIFLE, {12,69}})); plt1_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT1-SOL15"));
        plt1.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT1-SOL16"), "BLUE-PLT1-SOL16", Side::BLUE, ForceType::RIFLE, {12,70}})); plt1_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT1-SOL16"));
        plt1.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT1-SOL17"), "BLUE-PLT1-SOL17", Side::BLUE, ForceType::RIFLE, {13,70}})); plt1_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT1-SOL17"));
        plt1.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT1-SOL18"), "BLUE-PLT1-SOL18", Side::BLUE, ForceType::RIFLE, {14,70}})); plt1_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT1-SOL18"));
        plt1.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT1-SOL19"), "BLUE-PLT1-SOL19", Side::BLUE, ForceType::RIFLE, {15,66}})); plt1_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT1-SOL19"));
        plt1.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT1-SOL20"), "BLUE-PLT1-SOL20", Side::BLUE, ForceType::RIFLE, {16,66}})); plt1_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT1-SOL20"));
        plt1.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT1-SOL21"), "BLUE-PLT1-SOL21", Side::BLUE, ForceType::RIFLE, {16,67}})); plt1_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT1-SOL21"));
        plt1.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT1-SOL22"), "BLUE-PLT1-SOL22", Side::BLUE, ForceType::RIFLE, {16,68}})); plt1_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT1-SOL22"));
        plt1.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT1-SOL23"), "BLUE-PLT1-SOL23", Side::BLUE, ForceType::RIFLE, {16,69}})); plt1_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT1-SOL23"));
        plt1.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT1-SOL24"), "BLUE-PLT1-SOL24", Side::BLUE, ForceType::RIFLE, {16,70}})); plt1_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT1-SOL24"));
        plt1.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT1-SOL25"), "BLUE-PLT1-SOL25", Side::BLUE, ForceType::RIFLE, {17,67}})); plt1_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT1-SOL25"));

        // === BLUE PLT2 ===
        plt2.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT2-SOL01"), "BLUE-PLT2-SOL01", Side::BLUE, ForceType::RIFLE, {82,68}})); plt2_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT2-SOL01"));
        plt2.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT2-SOL02"), "BLUE-PLT2-SOL02", Side::BLUE, ForceType::RIFLE, {81,67}})); plt2_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT2-SOL02"));
        plt2.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT2-SOL03"), "BLUE-PLT2-SOL03", Side::BLUE, ForceType::RIFLE, {81,68}})); plt2_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT2-SOL03"));
        plt2.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT2-SOL04"), "BLUE-PLT2-SOL04", Side::BLUE, ForceType::RIFLE, {81,69}})); plt2_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT2-SOL04"));
        plt2.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT2-SOL05"), "BLUE-PLT2-SOL05", Side::BLUE, ForceType::RIFLE, {82,67}})); plt2_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT2-SOL05"));
        plt2.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT2-SOL06"), "BLUE-PLT2-SOL06", Side::BLUE, ForceType::RIFLE, {82,69}})); plt2_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT2-SOL06"));
        plt2.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT2-SOL07"), "BLUE-PLT2-SOL07", Side::BLUE, ForceType::RIFLE, {83,67}})); plt2_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT2-SOL07"));
        plt2.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT2-SOL08"), "BLUE-PLT2-SOL08", Side::BLUE, ForceType::RIFLE, {83,68}})); plt2_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT2-SOL08"));
        plt2.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT2-SOL09"), "BLUE-PLT2-SOL09", Side::BLUE, ForceType::RIFLE, {83,69}})); plt2_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT2-SOL09"));
        plt2.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT2-SOL10"), "BLUE-PLT2-SOL10", Side::BLUE, ForceType::RIFLE, {80,66}})); plt2_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT2-SOL10"));
        plt2.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT2-SOL11"), "BLUE-PLT2-SOL11", Side::BLUE, ForceType::RIFLE, {80,67}})); plt2_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT2-SOL11"));
        plt2.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT2-SOL12"), "BLUE-PLT2-SOL12", Side::BLUE, ForceType::RIFLE, {80,68}})); plt2_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT2-SOL12"));
        plt2.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT2-SOL13"), "BLUE-PLT2-SOL13", Side::BLUE, ForceType::RIFLE, {81,66}})); plt2_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT2-SOL13"));
        plt2.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT2-SOL14"), "BLUE-PLT2-SOL14", Side::BLUE, ForceType::RIFLE, {82,66}})); plt2_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT2-SOL14"));
        plt2.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT2-SOL15"), "BLUE-PLT2-SOL15", Side::BLUE, ForceType::RIFLE, {80,69}})); plt2_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT2-SOL15"));
        plt2.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT2-SOL16"), "BLUE-PLT2-SOL16", Side::BLUE, ForceType::RIFLE, {80,70}})); plt2_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT2-SOL16"));
        plt2.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT2-SOL17"), "BLUE-PLT2-SOL17", Side::BLUE, ForceType::RIFLE, {81,70}})); plt2_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT2-SOL17"));
        plt2.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT2-SOL18"), "BLUE-PLT2-SOL18", Side::BLUE, ForceType::RIFLE, {82,70}})); plt2_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT2-SOL18"));
        plt2.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT2-SOL19"), "BLUE-PLT2-SOL19", Side::BLUE, ForceType::RIFLE, {83,66}})); plt2_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT2-SOL19"));
        plt2.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT2-SOL20"), "BLUE-PLT2-SOL20", Side::BLUE, ForceType::RIFLE, {84,66}})); plt2_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT2-SOL20"));
        plt2.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT2-SOL21"), "BLUE-PLT2-SOL21", Side::BLUE, ForceType::RIFLE, {84,67}})); plt2_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT2-SOL21"));
        plt2.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT2-SOL22"), "BLUE-PLT2-SOL22", Side::BLUE, ForceType::RIFLE, {84,68}})); plt2_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT2-SOL22"));
        plt2.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT2-SOL23"), "BLUE-PLT2-SOL23", Side::BLUE, ForceType::RIFLE, {84,69}})); plt2_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT2-SOL23"));
        plt2.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT2-SOL24"), "BLUE-PLT2-SOL24", Side::BLUE, ForceType::RIFLE, {84,70}})); plt2_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT2-SOL24"));
        plt2.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT2-SOL25"), "BLUE-PLT2-SOL25", Side::BLUE, ForceType::RIFLE, {85,67}})); plt2_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT2-SOL25"));


        for (int i = 0; i < numOfPl1; ++i) {
            plt1[i]->SetParentModel(this);
            this->RegisterSubModel(plt1[i]);
        }
        for (int i = 0; i < numOfPl2; ++i) {
            plt2[i]->SetParentModel(this);
            this->RegisterSubModel(plt2[i]);
        }

        PlatoonLeader* plt1_leader = new PlatoonLeader(engine, env->RegisterEntityIdByName("BLUE-PLT1-LEADER"),&plt1_Ids); cmp_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT1-LEADER"));
        PlatoonLeader* plt2_leader = new PlatoonLeader(engine, env->RegisterEntityIdByName("BLUE-PLT2-LEADER"),&plt2_Ids); cmp_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT2-LEADER"));
        plt1_leader->SetModelName("BLUE-PLT1-LEADER");
        plt1_leader->SetParentModel(this);
        this->RegisterSubModel(plt1_leader);
        plt2_leader->SetModelName("BLUE-PLT2-LEADER");
        plt2_leader->SetParentModel(this);
        this->RegisterSubModel(plt2_leader);

        HQ* hq = new HQ(engine,&cmp_Ids);
        hq->SetModelName("BLUE-HQ");
        hq->SetParentModel(this);
        this->RegisterSubModel(hq);

        this->AddInputPort("Start");
        this->AddInputPort("RedFire");
        this->AddOutputPort("BlueFire");
        this->AddInputPort("RedPosition");
        this->AddOutputPort("BluePosition");
        

        // === 커플링 ===
        this->AddCoupling(this,"Start",hq,"Start",EIC);
        this->AddCoupling(hq,"CompanyOrd",plt1_leader,"CompanyOrd",IC);
        this->AddCoupling(hq,"CompanyOrd",plt2_leader,"CompanyOrd",IC);
        this->AddCoupling(plt1_leader,"PlatoonRep",hq,"InfantryRep",IC);
        this->AddCoupling(plt2_leader,"PlatoonRep",hq,"InfantryRep",IC);

        for (int i = 0; i < numOfPl1; ++i) {
            this->AddCoupling(this, "Start", plt1[i], "Start", EIC);
            this->AddCoupling(this, "RedFire",     plt1[i], "FireIn",     EIC);
            this->AddCoupling(plt1[i], "FireOut",      this, "BlueFire",     EOC);
            this->AddCoupling(plt1[i], "PositionOut", this, "BluePosition", EOC);
            this->AddCoupling(this,"RedPosition",plt1[i],"PositionIn",EIC);
            this->AddCoupling(plt1_leader,"PlatoonOrd",plt1[i],"PlatoonOrd",IC);
            this->AddCoupling(plt1[i],"SoldierRep",plt1_leader,"SoldierRep",IC);
            this->AddCoupling(plt1[i],"FireOut",plt1_leader,"FireFinished",IC);
        }
        for (int i = 0; i < numOfPl2; ++i) {
            this->AddCoupling(this, "Start", plt2[i], "Start", EIC);
            this->AddCoupling(this, "RedFire",     plt2[i], "FireIn",     EIC);
            this->AddCoupling(plt2[i], "FireOut",      this, "BlueFire",     EOC);
            this->AddCoupling(plt2[i], "PositionOut", this, "BluePosition", EOC);
            this->AddCoupling(this,"RedPosition",plt2[i],"PositionIn",EIC);
            this->AddCoupling(plt2_leader,"PlatoonOrd",plt2[i],"PlatoonOrd",IC);
            this->AddCoupling(plt2[i],"SoldierRep",plt2_leader,"SoldierRep",IC);
            this->AddCoupling(plt2[i],"FireOut",plt2_leader,"FireFinished",IC);
        }
    }
};