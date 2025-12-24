#pragma once
#include "DEVS/coupled_model.hpp"
#include "DEVS/logger.hpp"
#include "tdg_sim/common.hpp"
#include "tdg_sim/sim/environment/environment.hpp"
#include "tdg_sim/sim/infantry/soldier/soldier.hpp"
#include "tdg_sim/sim/infantry/platoon_leader/platoon_leader.hpp"
#include "tdg_sim/sim/hq/hq.hpp"
#include "tdg_sim/sim/artillery/artillery.hpp"
class BlueForce : public CoupledModel{
public:
    BlueForce(Engine* engine)
    : CoupledModel(engine)
    {
        std::vector<int> plt1_Ids; plt1_Ids.clear();
        std::vector<int> plt2_Ids; plt2_Ids.clear();
        std::vector<int> plt3_Ids; plt3_Ids.clear();
        std::vector<int> cmp_Ids; cmp_Ids.clear();

        std::vector<Soldier*> plt1;
        const int numOfPl1 = 25;
        plt1.reserve(numOfPl1);

        std::vector<Soldier*> plt2;
        const int numOfPl2 = 25;
        plt2.reserve(numOfPl2);

        std::vector<Soldier*> plt3;
        const int numOfPl3 = 25;
        plt3.reserve(numOfPl3);

        // === BLUE PLT1 ===
        plt1.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT1-SOL01"), "BLUE-PLT1-SOL01", SideType::BLUE, ForceType::RIFLE, {23,88}})); plt1_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT1-SOL01"));
        plt1.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT1-SOL02"), "BLUE-PLT1-SOL02", SideType::BLUE, ForceType::RIFLE, {24,88}})); plt1_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT1-SOL02"));
        plt1.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT1-SOL03"), "BLUE-PLT1-SOL03", SideType::BLUE, ForceType::RIFLE, {25,88}})); plt1_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT1-SOL03"));
        plt1.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT1-SOL04"), "BLUE-PLT1-SOL04", SideType::BLUE, ForceType::RIFLE, {26,88}})); plt1_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT1-SOL04"));
        plt1.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT1-SOL05"), "BLUE-PLT1-SOL05", SideType::BLUE, ForceType::RIFLE, {27,88}})); plt1_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT1-SOL05"));
        plt1.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT1-SOL06"), "BLUE-PLT1-SOL06", SideType::BLUE, ForceType::RIFLE, {23,89}})); plt1_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT1-SOL06"));
        plt1.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT1-SOL07"), "BLUE-PLT1-SOL07", SideType::BLUE, ForceType::RIFLE, {24,89}})); plt1_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT1-SOL07"));
        plt1.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT1-SOL08"), "BLUE-PLT1-SOL08", SideType::BLUE, ForceType::RIFLE, {25,89}})); plt1_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT1-SOL08"));
        plt1.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT1-SOL09"), "BLUE-PLT1-SOL09", SideType::BLUE, ForceType::RIFLE, {26,89}})); plt1_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT1-SOL09"));
        plt1.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT1-SOL10"), "BLUE-PLT1-SOL10", SideType::BLUE, ForceType::RIFLE, {27,89}})); plt1_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT1-SOL10"));
        plt1.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT1-SOL11"), "BLUE-PLT1-SOL11", SideType::BLUE, ForceType::RIFLE, {23,90}})); plt1_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT1-SOL11"));
        plt1.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT1-SOL12"), "BLUE-PLT1-SOL12", SideType::BLUE, ForceType::RIFLE, {24,90}})); plt1_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT1-SOL12"));
        plt1.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT1-SOL13"), "BLUE-PLT1-SOL13", SideType::BLUE, ForceType::RIFLE, {25,90}})); plt1_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT1-SOL13"));
        plt1.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT1-SOL14"), "BLUE-PLT1-SOL14", SideType::BLUE, ForceType::RIFLE, {26,90}})); plt1_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT1-SOL14"));
        plt1.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT1-SOL15"), "BLUE-PLT1-SOL15", SideType::BLUE, ForceType::RIFLE, {27,90}})); plt1_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT1-SOL15"));
        plt1.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT1-SOL16"), "BLUE-PLT1-SOL16", SideType::BLUE, ForceType::RIFLE, {23,91}})); plt1_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT1-SOL16"));
        plt1.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT1-SOL17"), "BLUE-PLT1-SOL17", SideType::BLUE, ForceType::RIFLE, {24,91}})); plt1_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT1-SOL17"));
        plt1.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT1-SOL18"), "BLUE-PLT1-SOL18", SideType::BLUE, ForceType::RIFLE, {25,91}})); plt1_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT1-SOL18"));
        plt1.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT1-SOL19"), "BLUE-PLT1-SOL19", SideType::BLUE, ForceType::RIFLE, {26,91}})); plt1_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT1-SOL19"));
        plt1.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT1-SOL20"), "BLUE-PLT1-SOL20", SideType::BLUE, ForceType::RIFLE, {27,91}})); plt1_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT1-SOL20"));
        plt1.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT1-SOL21"), "BLUE-PLT1-SOL21", SideType::BLUE, ForceType::RIFLE, {23,92}})); plt1_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT1-SOL21"));
        plt1.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT1-SOL22"), "BLUE-PLT1-SOL22", SideType::BLUE, ForceType::RIFLE, {24,92}})); plt1_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT1-SOL22"));
        plt1.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT1-SOL23"), "BLUE-PLT1-SOL23", SideType::BLUE, ForceType::RIFLE, {25,92}})); plt1_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT1-SOL23"));
        plt1.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT1-SOL24"), "BLUE-PLT1-SOL24", SideType::BLUE, ForceType::RIFLE, {26,92}})); plt1_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT1-SOL24"));
        plt1.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT1-SOL25"), "BLUE-PLT1-SOL25", SideType::BLUE, ForceType::RIFLE, {27,92}})); plt1_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT1-SOL25"));

        // === BLUE PLT2 ===
        plt2.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT2-SOL01"), "BLUE-PLT2-SOL01", SideType::BLUE, ForceType::RIFLE, {70,123}})); plt2_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT2-SOL01"));
        plt2.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT2-SOL02"), "BLUE-PLT2-SOL02", SideType::BLUE, ForceType::RIFLE, {71,123}})); plt2_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT2-SOL02"));
        plt2.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT2-SOL03"), "BLUE-PLT2-SOL03", SideType::BLUE, ForceType::RIFLE, {72,123}})); plt2_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT2-SOL03"));
        plt2.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT2-SOL04"), "BLUE-PLT2-SOL04", SideType::BLUE, ForceType::RIFLE, {73,123}})); plt2_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT2-SOL04"));
        plt2.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT2-SOL05"), "BLUE-PLT2-SOL05", SideType::BLUE, ForceType::RIFLE, {74,123}})); plt2_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT2-SOL05"));
        plt2.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT2-SOL06"), "BLUE-PLT2-SOL06", SideType::BLUE, ForceType::RIFLE, {70,124}})); plt2_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT2-SOL06"));
        plt2.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT2-SOL07"), "BLUE-PLT2-SOL07", SideType::BLUE, ForceType::RIFLE, {71,124}})); plt2_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT2-SOL07"));
        plt2.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT2-SOL08"), "BLUE-PLT2-SOL08", SideType::BLUE, ForceType::RIFLE, {72,124}})); plt2_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT2-SOL08"));
        plt2.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT2-SOL09"), "BLUE-PLT2-SOL09", SideType::BLUE, ForceType::RIFLE, {73,124}})); plt2_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT2-SOL09"));
        plt2.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT2-SOL10"), "BLUE-PLT2-SOL10", SideType::BLUE, ForceType::RIFLE, {74,124}})); plt2_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT2-SOL10"));
        plt2.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT2-SOL11"), "BLUE-PLT2-SOL11", SideType::BLUE, ForceType::RIFLE, {70,125}})); plt2_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT2-SOL11"));
        plt2.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT2-SOL12"), "BLUE-PLT2-SOL12", SideType::BLUE, ForceType::RIFLE, {71,125}})); plt2_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT2-SOL12"));
        plt2.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT2-SOL13"), "BLUE-PLT2-SOL13", SideType::BLUE, ForceType::RIFLE, {72,125}})); plt2_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT2-SOL13"));
        plt2.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT2-SOL14"), "BLUE-PLT2-SOL14", SideType::BLUE, ForceType::RIFLE, {73,125}})); plt2_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT2-SOL14"));
        plt2.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT2-SOL15"), "BLUE-PLT2-SOL15", SideType::BLUE, ForceType::RIFLE, {74,125}})); plt2_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT2-SOL15"));
        plt2.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT2-SOL16"), "BLUE-PLT2-SOL16", SideType::BLUE, ForceType::RIFLE, {70,126}})); plt2_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT2-SOL16"));
        plt2.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT2-SOL17"), "BLUE-PLT2-SOL17", SideType::BLUE, ForceType::RIFLE, {71,126}})); plt2_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT2-SOL17"));
        plt2.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT2-SOL18"), "BLUE-PLT2-SOL18", SideType::BLUE, ForceType::RIFLE, {72,126}})); plt2_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT2-SOL18"));
        plt2.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT2-SOL19"), "BLUE-PLT2-SOL19", SideType::BLUE, ForceType::RIFLE, {73,126}})); plt2_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT2-SOL19"));
        plt2.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT2-SOL20"), "BLUE-PLT2-SOL20", SideType::BLUE, ForceType::RIFLE, {74,126}})); plt2_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT2-SOL20"));
        plt2.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT2-SOL21"), "BLUE-PLT2-SOL21", SideType::BLUE, ForceType::RIFLE, {70,127}})); plt2_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT2-SOL21"));
        plt2.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT2-SOL22"), "BLUE-PLT2-SOL22", SideType::BLUE, ForceType::RIFLE, {71,127}})); plt2_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT2-SOL22"));
        plt2.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT2-SOL23"), "BLUE-PLT2-SOL23", SideType::BLUE, ForceType::RIFLE, {72,127}})); plt2_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT2-SOL23"));
        plt2.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT2-SOL24"), "BLUE-PLT2-SOL24", SideType::BLUE, ForceType::RIFLE, {73,127}})); plt2_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT2-SOL24"));
        plt2.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT2-SOL25"), "BLUE-PLT2-SOL25", SideType::BLUE, ForceType::RIFLE, {74,127}})); plt2_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT2-SOL25"));

        // === BLUE PLT3 ===
        plt3.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT3-SOL01"), "BLUE-PLT3-SOL01", SideType::BLUE, ForceType::RIFLE, {32,100}})); plt3_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT3-SOL01"));
        plt3.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT3-SOL02"), "BLUE-PLT3-SOL02", SideType::BLUE, ForceType::RIFLE, {33,100}})); plt3_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT3-SOL02"));
        plt3.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT3-SOL03"), "BLUE-PLT3-SOL03", SideType::BLUE, ForceType::RIFLE, {34,100}})); plt3_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT3-SOL03"));
        plt3.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT3-SOL04"), "BLUE-PLT3-SOL04", SideType::BLUE, ForceType::RIFLE, {35,100}})); plt3_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT3-SOL04"));
        plt3.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT3-SOL05"), "BLUE-PLT3-SOL05", SideType::BLUE, ForceType::RIFLE, {36,100}})); plt3_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT3-SOL05"));
        plt3.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT3-SOL06"), "BLUE-PLT3-SOL06", SideType::BLUE, ForceType::RIFLE, {32,101}})); plt3_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT3-SOL06"));
        plt3.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT3-SOL07"), "BLUE-PLT3-SOL07", SideType::BLUE, ForceType::RIFLE, {33,101}})); plt3_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT3-SOL07"));
        plt3.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT3-SOL08"), "BLUE-PLT3-SOL08", SideType::BLUE, ForceType::RIFLE, {34,101}})); plt3_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT3-SOL08"));
        plt3.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT3-SOL09"), "BLUE-PLT3-SOL09", SideType::BLUE, ForceType::RIFLE, {35,101}})); plt3_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT3-SOL09"));
        plt3.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT3-SOL10"), "BLUE-PLT3-SOL10", SideType::BLUE, ForceType::RIFLE, {36,101}})); plt3_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT3-SOL10"));
        plt3.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT3-SOL11"), "BLUE-PLT3-SOL11", SideType::BLUE, ForceType::RIFLE, {32,102}})); plt3_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT3-SOL11"));
        plt3.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT3-SOL12"), "BLUE-PLT3-SOL12", SideType::BLUE, ForceType::RIFLE, {33,102}})); plt3_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT3-SOL12"));
        plt3.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT3-SOL13"), "BLUE-PLT3-SOL13", SideType::BLUE, ForceType::RIFLE, {34,102}})); plt3_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT3-SOL13"));
        plt3.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT3-SOL14"), "BLUE-PLT3-SOL14", SideType::BLUE, ForceType::RIFLE, {35,102}})); plt3_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT3-SOL14"));
        plt3.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT3-SOL15"), "BLUE-PLT3-SOL15", SideType::BLUE, ForceType::RIFLE, {36,102}})); plt3_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT3-SOL15"));
        plt3.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT3-SOL16"), "BLUE-PLT3-SOL16", SideType::BLUE, ForceType::RIFLE, {32,103}})); plt3_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT3-SOL16"));
        plt3.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT3-SOL17"), "BLUE-PLT3-SOL17", SideType::BLUE, ForceType::RIFLE, {33,103}})); plt3_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT3-SOL17"));
        plt3.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT3-SOL18"), "BLUE-PLT3-SOL18", SideType::BLUE, ForceType::RIFLE, {34,103}})); plt3_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT3-SOL18"));
        plt3.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT3-SOL19"), "BLUE-PLT3-SOL19", SideType::BLUE, ForceType::RIFLE, {35,103}})); plt3_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT3-SOL19"));
        plt3.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT3-SOL20"), "BLUE-PLT3-SOL20", SideType::BLUE, ForceType::RIFLE, {36,103}})); plt3_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT3-SOL20"));
        plt3.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT3-SOL21"), "BLUE-PLT3-SOL21", SideType::BLUE, ForceType::RIFLE, {32,104}})); plt3_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT3-SOL21"));
        plt3.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT3-SOL22"), "BLUE-PLT3-SOL22", SideType::BLUE, ForceType::RIFLE, {33,104}})); plt3_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT3-SOL22"));
        plt3.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT3-SOL23"), "BLUE-PLT3-SOL23", SideType::BLUE, ForceType::RIFLE, {34,104}})); plt3_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT3-SOL23"));
        plt3.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT3-SOL24"), "BLUE-PLT3-SOL24", SideType::BLUE, ForceType::RIFLE, {35,104}})); plt3_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT3-SOL24"));
        plt3.push_back(new Soldier(engine, Entity{env->RegisterEntityIdByName("BLUE-PLT3-SOL25"), "BLUE-PLT3-SOL25", SideType::BLUE, ForceType::RIFLE, {36,104}})); plt3_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT3-SOL25"));

        for (int i = 0; i < numOfPl1; ++i) {
            plt1[i]->SetParentModel(this);
            this->RegisterSubModel(plt1[i]);
        }
        for (int i = 0; i < numOfPl2; ++i) {
            plt2[i]->SetParentModel(this);
            this->RegisterSubModel(plt2[i]);
        }
        for (int i = 0; i < numOfPl3; ++i) {
            plt3[i]->SetParentModel(this);
            this->RegisterSubModel(plt3[i]);
        }

        // 소대장 - 소대원보다 나중에 생성할 것
        PlatoonLeader* plt1_leader = new PlatoonLeader(engine, env->RegisterEntityIdByName("BLUE-PLT1-LEADER"), &plt1_Ids); cmp_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT1-LEADER"));
        PlatoonLeader* plt2_leader = new PlatoonLeader(engine, env->RegisterEntityIdByName("BLUE-PLT2-LEADER"), &plt2_Ids); cmp_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT2-LEADER"));
        PlatoonLeader* plt3_leader = new PlatoonLeader(engine, env->RegisterEntityIdByName("BLUE-PLT3-LEADER"), &plt3_Ids); cmp_Ids.push_back(env->QueryEntityIdByName("BLUE-PLT3-LEADER"));
        plt1_leader->SetModelName("BLUE-PLT1-LEADER");
        plt1_leader->SetParentModel(this);
        this->RegisterSubModel(plt1_leader);
        plt2_leader->SetModelName("BLUE-PLT2-LEADER");
        plt2_leader->SetParentModel(this);
        this->RegisterSubModel(plt2_leader);
        plt3_leader->SetModelName("BLUE-PLT3-LEADER");
        plt3_leader->SetParentModel(this);
        this->RegisterSubModel(plt3_leader);

        // 포병
        Artillery* art = new Artillery(engine, Entity{env->RegisterEntityIdByName("BLUE-ART"), "BLUE-ART", SideType::BLUE, ForceType::ARTILLERY, {10,130}}); cmp_Ids.push_back(env->QueryEntityIdByName("BLUE-ART"));
        art->SetParentModel(this);
        this->RegisterSubModel(art);

        // 중대장 - 중대원보다 나중에 생성할 것
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
        this->AddCoupling(plt1_leader,"PlatoonRep",hq,"InfantryRep",IC);
        this->AddCoupling(hq,"CompanyOrd",plt2_leader,"CompanyOrd",IC);
        this->AddCoupling(plt2_leader,"PlatoonRep",hq,"InfantryRep",IC);
        this->AddCoupling(hq,"CompanyOrd",plt3_leader,"CompanyOrd",IC);
        this->AddCoupling(plt3_leader,"PlatoonRep",hq,"InfantryRep",IC);
        this->AddCoupling(hq,"CompanyOrd",art,"CompanyOrd",IC);
        this->AddCoupling(art,"FireOut",this,"BlueFire",EOC);

        for (int i = 0; i < numOfPl1; ++i) {
            this->AddCoupling(this, "Start", plt1[i], "Start", EIC);
            this->AddCoupling(this, "RedFire", plt1[i], "FireIn", EIC);
            this->AddCoupling(plt1[i], "FireOut", this, "BlueFire", EOC);
            this->AddCoupling(plt1[i], "PositionOut", this, "BluePosition", EOC);
            this->AddCoupling(this, "RedPosition", plt1[i], "PositionIn", EIC);
            this->AddCoupling(plt1_leader, "PlatoonOrd", plt1[i], "PlatoonOrd", IC);
            this->AddCoupling(plt1[i], "SoldierRep", plt1_leader, "SoldierRep", IC);
            this->AddCoupling(plt1[i], "FireOut", plt1_leader, "FireFinished", IC);
            this->AddCoupling(art, "FireOut", plt1[i], "FireIn", IC);
        }
        for (int i = 0; i < numOfPl2; ++i) {
            this->AddCoupling(this, "Start", plt2[i], "Start", EIC);
            this->AddCoupling(this, "RedFire", plt2[i], "FireIn", EIC);
            this->AddCoupling(plt2[i], "FireOut", this, "BlueFire", EOC);
            this->AddCoupling(plt2[i], "PositionOut", this, "BluePosition", EOC);
            this->AddCoupling(this, "RedPosition", plt2[i], "PositionIn", EIC);
            this->AddCoupling(plt2_leader, "PlatoonOrd", plt2[i], "PlatoonOrd", IC);
            this->AddCoupling(plt2[i], "SoldierRep", plt2_leader, "SoldierRep", IC);
            this->AddCoupling(plt2[i], "FireOut", plt2_leader, "FireFinished", IC);
            this->AddCoupling(art, "FireOut", plt2[i], "FireIn", IC);
        }
        for (int i = 0; i < numOfPl3; ++i) {
            this->AddCoupling(this, "Start", plt3[i], "Start", EIC);
            this->AddCoupling(this, "RedFire", plt3[i], "FireIn", EIC);
            this->AddCoupling(plt3[i], "FireOut", this, "BlueFire", EOC);
            this->AddCoupling(plt3[i], "PositionOut", this, "BluePosition", EOC);
            this->AddCoupling(this, "RedPosition", plt3[i], "PositionIn", EIC);
            this->AddCoupling(plt3_leader, "PlatoonOrd", plt3[i], "PlatoonOrd", IC);
            this->AddCoupling(plt3[i], "SoldierRep", plt3_leader, "SoldierRep", IC);
            this->AddCoupling(plt3[i], "FireOut", plt3_leader, "FireFinished", IC);
            this->AddCoupling(art, "FireOut", plt3[i], "FireIn", IC);
        }
    }
};