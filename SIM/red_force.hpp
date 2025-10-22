#pragma once
#include "DEVS/coupled_model.hpp"
#include "common.hpp"
#include "DEVS/logger.hpp"
#include "SIM/Environment/environment.hpp"
#include "SIM/Infantry/Soldier/soldier.hpp"
#include "SIM/Infantry/PlatoonLeader/platoon_leader.hpp"
#include "SIM/HQ/hq.hpp"

class RedForce : public CoupledModel{
public:
    RedForce(Engine* engine)
    : CoupledModel(engine)
    {
        std::vector<int> plt1_Ids; plt1_Ids.clear();
        std::vector<int> plt2_Ids; plt2_Ids.clear();
        std::vector<int> cmp_Ids; cmp_Ids.clear();

        std::vector<Soldier*> plt1;
        const int numOfPl1 = 25;
        plt1.reserve(numOfPl1);

        std::vector<Soldier*> plt2;
        const int numOfPl2 = 25;
        plt2.reserve(numOfPl2);

        // === RED PLT1 ===
        plt1.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("RED-PLT1-SOL01"), "RED-PLT1-SOL01", Side::RED, ForceType::RIFLE, {85,31}})); plt1_Ids.push_back(env->QueryEntityIdByName("RED-PLT1-SOL01"));
        plt1.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("RED-PLT1-SOL02"), "RED-PLT1-SOL02", Side::RED, ForceType::RIFLE, {84,30}})); plt1_Ids.push_back(env->QueryEntityIdByName("RED-PLT1-SOL02"));
        plt1.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("RED-PLT1-SOL03"), "RED-PLT1-SOL03", Side::RED, ForceType::RIFLE, {84,31}})); plt1_Ids.push_back(env->QueryEntityIdByName("RED-PLT1-SOL03"));
        plt1.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("RED-PLT1-SOL04"), "RED-PLT1-SOL04", Side::RED, ForceType::RIFLE, {84,32}})); plt1_Ids.push_back(env->QueryEntityIdByName("RED-PLT1-SOL04"));
        plt1.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("RED-PLT1-SOL05"), "RED-PLT1-SOL05", Side::RED, ForceType::RIFLE, {85,30}})); plt1_Ids.push_back(env->QueryEntityIdByName("RED-PLT1-SOL05"));
        plt1.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("RED-PLT1-SOL06"), "RED-PLT1-SOL06", Side::RED, ForceType::RIFLE, {85,32}})); plt1_Ids.push_back(env->QueryEntityIdByName("RED-PLT1-SOL06"));
        plt1.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("RED-PLT1-SOL07"), "RED-PLT1-SOL07", Side::RED, ForceType::RIFLE, {86,30}})); plt1_Ids.push_back(env->QueryEntityIdByName("RED-PLT1-SOL07"));
        plt1.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("RED-PLT1-SOL08"), "RED-PLT1-SOL08", Side::RED, ForceType::RIFLE, {86,31}})); plt1_Ids.push_back(env->QueryEntityIdByName("RED-PLT1-SOL08"));
        plt1.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("RED-PLT1-SOL09"), "RED-PLT1-SOL09", Side::RED, ForceType::RIFLE, {86,32}})); plt1_Ids.push_back(env->QueryEntityIdByName("RED-PLT1-SOL09"));
        plt1.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("RED-PLT1-SOL10"), "RED-PLT1-SOL10", Side::RED, ForceType::RIFLE, {83,29}})); plt1_Ids.push_back(env->QueryEntityIdByName("RED-PLT1-SOL10"));
        plt1.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("RED-PLT1-SOL11"), "RED-PLT1-SOL11", Side::RED, ForceType::RIFLE, {83,30}})); plt1_Ids.push_back(env->QueryEntityIdByName("RED-PLT1-SOL11"));
        plt1.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("RED-PLT1-SOL12"), "RED-PLT1-SOL12", Side::RED, ForceType::RIFLE, {83,31}})); plt1_Ids.push_back(env->QueryEntityIdByName("RED-PLT1-SOL12"));
        plt1.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("RED-PLT1-SOL13"), "RED-PLT1-SOL13", Side::RED, ForceType::RIFLE, {84,29}})); plt1_Ids.push_back(env->QueryEntityIdByName("RED-PLT1-SOL13"));
        plt1.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("RED-PLT1-SOL14"), "RED-PLT1-SOL14", Side::RED, ForceType::RIFLE, {85,29}})); plt1_Ids.push_back(env->QueryEntityIdByName("RED-PLT1-SOL14"));
        plt1.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("RED-PLT1-SOL15"), "RED-PLT1-SOL15", Side::RED, ForceType::RIFLE, {83,32}})); plt1_Ids.push_back(env->QueryEntityIdByName("RED-PLT1-SOL15"));
        plt1.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("RED-PLT1-SOL16"), "RED-PLT1-SOL16", Side::RED, ForceType::RIFLE, {83,33}})); plt1_Ids.push_back(env->QueryEntityIdByName("RED-PLT1-SOL16"));
        plt1.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("RED-PLT1-SOL17"), "RED-PLT1-SOL17", Side::RED, ForceType::RIFLE, {84,33}})); plt1_Ids.push_back(env->QueryEntityIdByName("RED-PLT1-SOL17"));
        plt1.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("RED-PLT1-SOL18"), "RED-PLT1-SOL18", Side::RED, ForceType::RIFLE, {85,33}})); plt1_Ids.push_back(env->QueryEntityIdByName("RED-PLT1-SOL18"));
        plt1.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("RED-PLT1-SOL19"), "RED-PLT1-SOL19", Side::RED, ForceType::RIFLE, {86,29}})); plt1_Ids.push_back(env->QueryEntityIdByName("RED-PLT1-SOL19"));
        plt1.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("RED-PLT1-SOL20"), "RED-PLT1-SOL20", Side::RED, ForceType::RIFLE, {87,29}})); plt1_Ids.push_back(env->QueryEntityIdByName("RED-PLT1-SOL20"));
        plt1.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("RED-PLT1-SOL21"), "RED-PLT1-SOL21", Side::RED, ForceType::RIFLE, {87,30}})); plt1_Ids.push_back(env->QueryEntityIdByName("RED-PLT1-SOL21"));
        plt1.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("RED-PLT1-SOL22"), "RED-PLT1-SOL22", Side::RED, ForceType::RIFLE, {87,31}})); plt1_Ids.push_back(env->QueryEntityIdByName("RED-PLT1-SOL22"));
        plt1.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("RED-PLT1-SOL23"), "RED-PLT1-SOL23", Side::RED, ForceType::RIFLE, {87,32}})); plt1_Ids.push_back(env->QueryEntityIdByName("RED-PLT1-SOL23"));
        plt1.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("RED-PLT1-SOL24"), "RED-PLT1-SOL24", Side::RED, ForceType::RIFLE, {87,33}})); plt1_Ids.push_back(env->QueryEntityIdByName("RED-PLT1-SOL24"));
        plt1.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("RED-PLT1-SOL25"), "RED-PLT1-SOL25", Side::RED, ForceType::RIFLE, {88,30}})); plt1_Ids.push_back(env->QueryEntityIdByName("RED-PLT1-SOL25"));

        // === RED PLT2 ===
        plt2.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("RED-PLT2-SOL01"), "RED-PLT2-SOL01", Side::RED, ForceType::RIFLE, {76,55}})); plt2_Ids.push_back(env->QueryEntityIdByName("RED-PLT2-SOL01"));
        plt2.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("RED-PLT2-SOL02"), "RED-PLT2-SOL02", Side::RED, ForceType::RIFLE, {75,54}})); plt2_Ids.push_back(env->QueryEntityIdByName("RED-PLT2-SOL02"));
        plt2.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("RED-PLT2-SOL03"), "RED-PLT2-SOL03", Side::RED, ForceType::RIFLE, {75,55}})); plt2_Ids.push_back(env->QueryEntityIdByName("RED-PLT2-SOL03"));
        plt2.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("RED-PLT2-SOL04"), "RED-PLT2-SOL04", Side::RED, ForceType::RIFLE, {75,56}})); plt2_Ids.push_back(env->QueryEntityIdByName("RED-PLT2-SOL04"));
        plt2.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("RED-PLT2-SOL05"), "RED-PLT2-SOL05", Side::RED, ForceType::RIFLE, {76,54}})); plt2_Ids.push_back(env->QueryEntityIdByName("RED-PLT2-SOL05"));
        plt2.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("RED-PLT2-SOL06"), "RED-PLT2-SOL06", Side::RED, ForceType::RIFLE, {76,56}})); plt2_Ids.push_back(env->QueryEntityIdByName("RED-PLT2-SOL06"));
        plt2.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("RED-PLT2-SOL07"), "RED-PLT2-SOL07", Side::RED, ForceType::RIFLE, {77,54}})); plt2_Ids.push_back(env->QueryEntityIdByName("RED-PLT2-SOL07"));
        plt2.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("RED-PLT2-SOL08"), "RED-PLT2-SOL08", Side::RED, ForceType::RIFLE, {77,55}})); plt2_Ids.push_back(env->QueryEntityIdByName("RED-PLT2-SOL08"));
        plt2.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("RED-PLT2-SOL09"), "RED-PLT2-SOL09", Side::RED, ForceType::RIFLE, {77,56}})); plt2_Ids.push_back(env->QueryEntityIdByName("RED-PLT2-SOL09"));
        plt2.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("RED-PLT2-SOL10"), "RED-PLT2-SOL10", Side::RED, ForceType::RIFLE, {74,53}})); plt2_Ids.push_back(env->QueryEntityIdByName("RED-PLT2-SOL10"));
        plt2.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("RED-PLT2-SOL11"), "RED-PLT2-SOL11", Side::RED, ForceType::RIFLE, {74,54}})); plt2_Ids.push_back(env->QueryEntityIdByName("RED-PLT2-SOL11"));
        plt2.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("RED-PLT2-SOL12"), "RED-PLT2-SOL12", Side::RED, ForceType::RIFLE, {74,55}})); plt2_Ids.push_back(env->QueryEntityIdByName("RED-PLT2-SOL12"));
        plt2.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("RED-PLT2-SOL13"), "RED-PLT2-SOL13", Side::RED, ForceType::RIFLE, {75,53}})); plt2_Ids.push_back(env->QueryEntityIdByName("RED-PLT2-SOL13"));
        plt2.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("RED-PLT2-SOL14"), "RED-PLT2-SOL14", Side::RED, ForceType::RIFLE, {76,53}})); plt2_Ids.push_back(env->QueryEntityIdByName("RED-PLT2-SOL14"));
        plt2.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("RED-PLT2-SOL15"), "RED-PLT2-SOL15", Side::RED, ForceType::RIFLE, {74,56}})); plt2_Ids.push_back(env->QueryEntityIdByName("RED-PLT2-SOL15"));
        plt2.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("RED-PLT2-SOL16"), "RED-PLT2-SOL16", Side::RED, ForceType::RIFLE, {74,57}})); plt2_Ids.push_back(env->QueryEntityIdByName("RED-PLT2-SOL16"));
        plt2.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("RED-PLT2-SOL17"), "RED-PLT2-SOL17", Side::RED, ForceType::RIFLE, {75,57}})); plt2_Ids.push_back(env->QueryEntityIdByName("RED-PLT2-SOL17"));
        plt2.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("RED-PLT2-SOL18"), "RED-PLT2-SOL18", Side::RED, ForceType::RIFLE, {76,57}})); plt2_Ids.push_back(env->QueryEntityIdByName("RED-PLT2-SOL18"));
        plt2.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("RED-PLT2-SOL19"), "RED-PLT2-SOL19", Side::RED, ForceType::RIFLE, {77,53}})); plt2_Ids.push_back(env->QueryEntityIdByName("RED-PLT2-SOL19"));
        plt2.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("RED-PLT2-SOL20"), "RED-PLT2-SOL20", Side::RED, ForceType::RIFLE, {77,57}})); plt2_Ids.push_back(env->QueryEntityIdByName("RED-PLT2-SOL20"));
        plt2.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("RED-PLT2-SOL21"), "RED-PLT2-SOL21", Side::RED, ForceType::RIFLE, {78,53}})); plt2_Ids.push_back(env->QueryEntityIdByName("RED-PLT2-SOL21"));
        plt2.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("RED-PLT2-SOL22"), "RED-PLT2-SOL22", Side::RED, ForceType::RIFLE, {78,54}})); plt2_Ids.push_back(env->QueryEntityIdByName("RED-PLT2-SOL22"));
        plt2.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("RED-PLT2-SOL23"), "RED-PLT2-SOL23", Side::RED, ForceType::RIFLE, {78,55}})); plt2_Ids.push_back(env->QueryEntityIdByName("RED-PLT2-SOL23"));
        plt2.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("RED-PLT2-SOL24"), "RED-PLT2-SOL24", Side::RED, ForceType::RIFLE, {78,56}})); plt2_Ids.push_back(env->QueryEntityIdByName("RED-PLT2-SOL24"));
        plt2.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("RED-PLT2-SOL25"), "RED-PLT2-SOL25", Side::RED, ForceType::RIFLE, {78,57}})); plt2_Ids.push_back(env->QueryEntityIdByName("RED-PLT2-SOL25"));

        for (int i = 0; i < numOfPl1; ++i) {
            plt1[i]->SetParentModel(this);
            this->RegisterSubModel(plt1[i]);
        }
        for (int i = 0; i < numOfPl2; ++i) {
            plt2[i]->SetParentModel(this);
            this->RegisterSubModel(plt2[i]);
        }

        PlatoonLeader* plt1_leader = new PlatoonLeader(engine, env->RegisterEntityIdByName("RED-PLT1-LEADER"), &plt1_Ids); cmp_Ids.push_back(env->QueryEntityIdByName("RED-PLT1-LEADER"));
        PlatoonLeader* plt2_leader = new PlatoonLeader(engine, env->RegisterEntityIdByName("RED-PLT2-LEADER"), &plt2_Ids); cmp_Ids.push_back(env->QueryEntityIdByName("RED-PLT2-LEADER"));
        plt1_leader->SetModelName("RED-PLT1-LEADER");
        plt1_leader->SetParentModel(this);
        this->RegisterSubModel(plt1_leader);
        plt2_leader->SetModelName("RED-PLT2-LEADER");
        plt2_leader->SetParentModel(this);
        this->RegisterSubModel(plt2_leader);

        HQ* hq = new HQ(engine, &cmp_Ids, Side::RED);
        hq->SetModelName("RED-HQ");
        hq->SetParentModel(this);
        this->RegisterSubModel(hq);

        this->AddInputPort("Start");
        this->AddInputPort("BlueFire");
        this->AddOutputPort("RedFire");
        this->AddInputPort("BluePosition");
        this->AddOutputPort("RedPosition");

        this->AddCoupling(this,"Start",hq,"Start",EIC);
        this->AddCoupling(hq,"CompanyOrd",plt1_leader,"CompanyOrd",IC);
        this->AddCoupling(hq,"CompanyOrd",plt2_leader,"CompanyOrd",IC);
        this->AddCoupling(plt1_leader,"PlatoonRep",hq,"InfantryRep",IC);
        this->AddCoupling(plt2_leader,"PlatoonRep",hq,"InfantryRep",IC);

        for (int i = 0; i < numOfPl1; ++i) {
            this->AddCoupling(this, "Start", plt1[i], "Start", EIC);
            this->AddCoupling(this,"BlueFire",plt1[i],"FireIn",EIC);
            this->AddCoupling(plt1[i],"FireOut",this,"RedFire",EOC);
            this->AddCoupling(plt1[i],"PositionOut",this,"RedPosition",EOC);
            this->AddCoupling(this,"BluePosition",plt1[i],"PositionIn",EIC);
            this->AddCoupling(plt1_leader,"PlatoonOrd",plt1[i],"PlatoonOrd",IC);
            this->AddCoupling(plt1[i],"SoldierRep",plt1_leader,"SoldierRep",IC);
            this->AddCoupling(plt1[i],"FireOut",plt1_leader,"FireFinished",IC);
        }
        for (int i = 0; i < numOfPl2; ++i) {
            this->AddCoupling(this, "Start", plt2[i], "Start", EIC);
            this->AddCoupling(this,"BlueFire",plt2[i],"FireIn",EIC);
            this->AddCoupling(plt2[i],"FireOut",this,"RedFire",EOC);
            this->AddCoupling(plt2[i],"PositionOut",this,"RedPosition",EOC);
            this->AddCoupling(this,"BluePosition",plt2[i],"PositionIn",EIC);
            this->AddCoupling(plt2_leader,"PlatoonOrd",plt2[i],"PlatoonOrd",IC);
            this->AddCoupling(plt2[i],"SoldierRep",plt2_leader,"SoldierRep",IC);
            this->AddCoupling(plt2[i],"FireOut",plt2_leader,"FireFinished",IC);
        }

    //     // Append RED tanks from map.json and couple to leaders and I/O
    //     try {
    //         std::ifstream ifs("map.json");
    //         if (ifs.is_open()) {
    //             nlohmann::json j; ifs >> j;
    //             const char* keysEnt[] = {"units", "entities"};
    //             for (const char* key : keysEnt) {
    //                 if (!j.contains(key) || !j[key].is_array()) continue;
    //                 for (const auto& u : j[key]){
    //                     std::string type = u.value("type", std::string{"rifle"});
    //                     std::string side = u.value("side", std::string{"RED"});
    //                     if (!(type == "tank" || type == "TANK")) continue;
    //                     if (!(side == "red" || side == "RED")) continue;
    //                     std::string uid = u.value("uid", std::string{});
    //                     int x=0,y=0;
    //                     if (u.contains("x") && u.contains("y")) { x = u["x"].get<int>(); y = u["y"].get<int>(); }
    //                     else if (u.contains("pos") && u["pos"].is_array() && u["pos"].size()>=2) { x = u["pos"][0].get<int>(); y = u["pos"][1].get<int>(); }
    //                     int id = env->RegisterEntityIdByName(uid);
    //                     Tank* t = new Tank(engine, Entity{id, uid, Side::RED, ForceType::TANK, {x,y}});
    //                     t->SetModelName(uid);
    //                     t->SetParentModel(this);
    //                     this->RegisterSubModel(t);

    //                     // Force-level couplings
    //                     this->AddCoupling(this, "Start", t, "Start", EIC);
    //                     this->AddCoupling(this, "BlueFire",     t, "FireIn",     EIC);
    //                     this->AddCoupling(t, "FireOut",      this, "RedFire",     EOC);
    //                     this->AddCoupling(t, "PositionOut", this, "RedPosition", EOC);
    //                     this->AddCoupling(this,"BluePosition",t,"PositionIn",EIC);

    //                     // Attach to a leader (use PLT1 by default)
    //                     this->AddCoupling(plt1_leader,"PlatoonOrd",t,"PlatoonOrd",IC);
    //                     this->AddCoupling(t,"SoldierRep",plt1_leader,"SoldierRep",IC);
    //                     this->AddCoupling(t,"FireOut",plt1_leader,"FireFinished",IC);
    //                 }
    //                 break;
    //             }
    //         }
    //     } catch(...) { /* ignore */ }
    }
};
