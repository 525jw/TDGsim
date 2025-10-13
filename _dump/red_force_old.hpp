#pragma once
#include "DEVS/coupled_model.hpp"
#include "common.hpp"
#include "DEVS/logger.hpp"
#include "SIM/Environment/environment.hpp"
#include "red_cgf.hpp"

class RedForce : public CoupledModel{
public:
    RedForce(Engine* engine)
    : CoupledModel(engine)
    {
        // 생성
        std::vector<RedCgf*> plt1;
        int numOfPl1 = 25;
        plt1.reserve(numOfPl1);

        std::vector<RedCgf*> plt2;
        int numOfPl2 = 25;
        plt1.reserve(numOfPl2);

        // === RED PLT1 ===
        plt1.push_back(new RedCgf(engine, Entity{env->RegisterEntityIdByName("RED-PLT1-SOL01"), "RED-PLT1-SOL01", Side::RED, ForceType::RIFLE, {85,31}}));
        plt1.push_back(new RedCgf(engine, Entity{env->RegisterEntityIdByName("RED-PLT1-SOL02"), "RED-PLT1-SOL02", Side::RED, ForceType::RIFLE, {84,30}}));
        plt1.push_back(new RedCgf(engine, Entity{env->RegisterEntityIdByName("RED-PLT1-SOL03"), "RED-PLT1-SOL03", Side::RED, ForceType::RIFLE, {84,31}}));
        plt1.push_back(new RedCgf(engine, Entity{env->RegisterEntityIdByName("RED-PLT1-SOL04"), "RED-PLT1-SOL04", Side::RED, ForceType::RIFLE, {84,32}}));
        plt1.push_back(new RedCgf(engine, Entity{env->RegisterEntityIdByName("RED-PLT1-SOL05"), "RED-PLT1-SOL05", Side::RED, ForceType::RIFLE, {85,30}}));
        plt1.push_back(new RedCgf(engine, Entity{env->RegisterEntityIdByName("RED-PLT1-SOL06"), "RED-PLT1-SOL06", Side::RED, ForceType::RIFLE, {85,32}}));
        plt1.push_back(new RedCgf(engine, Entity{env->RegisterEntityIdByName("RED-PLT1-SOL07"), "RED-PLT1-SOL07", Side::RED, ForceType::RIFLE, {86,30}}));
        plt1.push_back(new RedCgf(engine, Entity{env->RegisterEntityIdByName("RED-PLT1-SOL08"), "RED-PLT1-SOL08", Side::RED, ForceType::RIFLE, {86,31}}));
        plt1.push_back(new RedCgf(engine, Entity{env->RegisterEntityIdByName("RED-PLT1-SOL09"), "RED-PLT1-SOL09", Side::RED, ForceType::RIFLE, {86,32}}));
        plt1.push_back(new RedCgf(engine, Entity{env->RegisterEntityIdByName("RED-PLT1-SOL10"), "RED-PLT1-SOL10", Side::RED, ForceType::RIFLE, {83,29}}));
        plt1.push_back(new RedCgf(engine, Entity{env->RegisterEntityIdByName("RED-PLT1-SOL11"), "RED-PLT1-SOL11", Side::RED, ForceType::RIFLE, {83,30}}));
        plt1.push_back(new RedCgf(engine, Entity{env->RegisterEntityIdByName("RED-PLT1-SOL12"), "RED-PLT1-SOL12", Side::RED, ForceType::RIFLE, {83,31}}));
        plt1.push_back(new RedCgf(engine, Entity{env->RegisterEntityIdByName("RED-PLT1-SOL13"), "RED-PLT1-SOL13", Side::RED, ForceType::RIFLE, {84,29}}));
        plt1.push_back(new RedCgf(engine, Entity{env->RegisterEntityIdByName("RED-PLT1-SOL14"), "RED-PLT1-SOL14", Side::RED, ForceType::RIFLE, {85,29}}));
        plt1.push_back(new RedCgf(engine, Entity{env->RegisterEntityIdByName("RED-PLT1-SOL15"), "RED-PLT1-SOL15", Side::RED, ForceType::RIFLE, {83,32}}));
        plt1.push_back(new RedCgf(engine, Entity{env->RegisterEntityIdByName("RED-PLT1-SOL16"), "RED-PLT1-SOL16", Side::RED, ForceType::RIFLE, {83,33}}));
        plt1.push_back(new RedCgf(engine, Entity{env->RegisterEntityIdByName("RED-PLT1-SOL17"), "RED-PLT1-SOL17", Side::RED, ForceType::RIFLE, {84,33}}));
        plt1.push_back(new RedCgf(engine, Entity{env->RegisterEntityIdByName("RED-PLT1-SOL18"), "RED-PLT1-SOL18", Side::RED, ForceType::RIFLE, {85,33}}));
        plt1.push_back(new RedCgf(engine, Entity{env->RegisterEntityIdByName("RED-PLT1-SOL19"), "RED-PLT1-SOL19", Side::RED, ForceType::RIFLE, {86,29}}));
        plt1.push_back(new RedCgf(engine, Entity{env->RegisterEntityIdByName("RED-PLT1-SOL20"), "RED-PLT1-SOL20", Side::RED, ForceType::RIFLE, {87,29}}));
        plt1.push_back(new RedCgf(engine, Entity{env->RegisterEntityIdByName("RED-PLT1-SOL21"), "RED-PLT1-SOL21", Side::RED, ForceType::RIFLE, {87,30}}));
        plt1.push_back(new RedCgf(engine, Entity{env->RegisterEntityIdByName("RED-PLT1-SOL22"), "RED-PLT1-SOL22", Side::RED, ForceType::RIFLE, {87,31}}));
        plt1.push_back(new RedCgf(engine, Entity{env->RegisterEntityIdByName("RED-PLT1-SOL23"), "RED-PLT1-SOL23", Side::RED, ForceType::RIFLE, {87,32}}));
        plt1.push_back(new RedCgf(engine, Entity{env->RegisterEntityIdByName("RED-PLT1-SOL24"), "RED-PLT1-SOL24", Side::RED, ForceType::RIFLE, {87,33}}));
        plt1.push_back(new RedCgf(engine, Entity{env->RegisterEntityIdByName("RED-PLT1-SOL25"), "RED-PLT1-SOL25", Side::RED, ForceType::RIFLE, {88,30}}));

        // === RED PLT2 ===
        plt2.push_back(new RedCgf(engine, Entity{env->RegisterEntityIdByName("RED-PLT2-SOL01"), "RED-PLT2-SOL01", Side::RED, ForceType::RIFLE, {76,55}}));
        plt2.push_back(new RedCgf(engine, Entity{env->RegisterEntityIdByName("RED-PLT2-SOL02"), "RED-PLT2-SOL02", Side::RED, ForceType::RIFLE, {75,54}}));
        plt2.push_back(new RedCgf(engine, Entity{env->RegisterEntityIdByName("RED-PLT2-SOL03"), "RED-PLT2-SOL03", Side::RED, ForceType::RIFLE, {75,55}}));
        plt2.push_back(new RedCgf(engine, Entity{env->RegisterEntityIdByName("RED-PLT2-SOL04"), "RED-PLT2-SOL04", Side::RED, ForceType::RIFLE, {75,56}}));
        plt2.push_back(new RedCgf(engine, Entity{env->RegisterEntityIdByName("RED-PLT2-SOL05"), "RED-PLT2-SOL05", Side::RED, ForceType::RIFLE, {76,54}}));
        plt2.push_back(new RedCgf(engine, Entity{env->RegisterEntityIdByName("RED-PLT2-SOL06"), "RED-PLT2-SOL06", Side::RED, ForceType::RIFLE, {76,56}}));
        plt2.push_back(new RedCgf(engine, Entity{env->RegisterEntityIdByName("RED-PLT2-SOL07"), "RED-PLT2-SOL07", Side::RED, ForceType::RIFLE, {77,54}}));
        plt2.push_back(new RedCgf(engine, Entity{env->RegisterEntityIdByName("RED-PLT2-SOL08"), "RED-PLT2-SOL08", Side::RED, ForceType::RIFLE, {77,55}}));
        plt2.push_back(new RedCgf(engine, Entity{env->RegisterEntityIdByName("RED-PLT2-SOL09"), "RED-PLT2-SOL09", Side::RED, ForceType::RIFLE, {77,56}}));
        plt2.push_back(new RedCgf(engine, Entity{env->RegisterEntityIdByName("RED-PLT2-SOL10"), "RED-PLT2-SOL10", Side::RED, ForceType::RIFLE, {74,53}}));
        plt2.push_back(new RedCgf(engine, Entity{env->RegisterEntityIdByName("RED-PLT2-SOL11"), "RED-PLT2-SOL11", Side::RED, ForceType::RIFLE, {74,54}}));
        plt2.push_back(new RedCgf(engine, Entity{env->RegisterEntityIdByName("RED-PLT2-SOL12"), "RED-PLT2-SOL12", Side::RED, ForceType::RIFLE, {74,55}}));
        plt2.push_back(new RedCgf(engine, Entity{env->RegisterEntityIdByName("RED-PLT2-SOL13"), "RED-PLT2-SOL13", Side::RED, ForceType::RIFLE, {75,53}}));
        plt2.push_back(new RedCgf(engine, Entity{env->RegisterEntityIdByName("RED-PLT2-SOL14"), "RED-PLT2-SOL14", Side::RED, ForceType::RIFLE, {76,53}}));
        plt2.push_back(new RedCgf(engine, Entity{env->RegisterEntityIdByName("RED-PLT2-SOL15"), "RED-PLT2-SOL15", Side::RED, ForceType::RIFLE, {74,56}}));
        plt2.push_back(new RedCgf(engine, Entity{env->RegisterEntityIdByName("RED-PLT2-SOL16"), "RED-PLT2-SOL16", Side::RED, ForceType::RIFLE, {74,57}}));
        plt2.push_back(new RedCgf(engine, Entity{env->RegisterEntityIdByName("RED-PLT2-SOL17"), "RED-PLT2-SOL17", Side::RED, ForceType::RIFLE, {75,57}}));
        plt2.push_back(new RedCgf(engine, Entity{env->RegisterEntityIdByName("RED-PLT2-SOL18"), "RED-PLT2-SOL18", Side::RED, ForceType::RIFLE, {76,57}}));
        plt2.push_back(new RedCgf(engine, Entity{env->RegisterEntityIdByName("RED-PLT2-SOL19"), "RED-PLT2-SOL19", Side::RED, ForceType::RIFLE, {77,53}}));
        plt2.push_back(new RedCgf(engine, Entity{env->RegisterEntityIdByName("RED-PLT2-SOL20"), "RED-PLT2-SOL20", Side::RED, ForceType::RIFLE, {77,57}}));
        plt2.push_back(new RedCgf(engine, Entity{env->RegisterEntityIdByName("RED-PLT2-SOL21"), "RED-PLT2-SOL21", Side::RED, ForceType::RIFLE, {78,53}}));
        plt2.push_back(new RedCgf(engine, Entity{env->RegisterEntityIdByName("RED-PLT2-SOL22"), "RED-PLT2-SOL22", Side::RED, ForceType::RIFLE, {78,54}}));
        plt2.push_back(new RedCgf(engine, Entity{env->RegisterEntityIdByName("RED-PLT2-SOL23"), "RED-PLT2-SOL23", Side::RED, ForceType::RIFLE, {78,55}}));
        plt2.push_back(new RedCgf(engine, Entity{env->RegisterEntityIdByName("RED-PLT2-SOL24"), "RED-PLT2-SOL24", Side::RED, ForceType::RIFLE, {78,56}}));
        plt2.push_back(new RedCgf(engine, Entity{env->RegisterEntityIdByName("RED-PLT2-SOL25"), "RED-PLT2-SOL25", Side::RED, ForceType::RIFLE, {78,57}}));

        for(int i=0;i<numOfPl1;i++){
            this->RegisterSubModel(plt1[i]);                // 자식모델 목록에 등록
            plt1[i]->SetParentModel(this);                  // 부모를 this로 설정
        }
        for(int i=0;i<numOfPl2;i++){
            this->RegisterSubModel(plt2[i]);                // 자식모델 목록에 등록
            plt2[i]->SetParentModel(this);                  // 부모를 this로 설정
        }
        // IO port 설정
        this->AddInputPort("Start");
        this->AddInputPort("BlueFire");
        this->AddOutputPort("RedFire");
        // this->AddInputPort("BluePosition");
        // this->AddOutputPort("RedPosition");
        
        for(int i=0;i<numOfPl1;i++){
            this->AddCoupling(this, "Start", plt1[i], "Start", EIC);
            this->AddCoupling(this,"BlueFire",plt1[i],"FireIn",EIC);
            this->AddCoupling(plt1[i],"FireOut",this,"RedFire",EOC);
            // this->AddCoupling(this,"BluePosition",plt1[i],"PositionIn",EIC);
            // this->AddCoupling(plt1[i],"PositionOut",this,"RedPosition",EOC);
        }
        for(int i=0;i<numOfPl2;i++){
            this->AddCoupling(this, "Start", plt2[i], "Start", EIC);
            this->AddCoupling(this,"BlueFire",plt2[i],"FireIn",EIC);
            this->AddCoupling(plt2[i],"FireOut",this,"RedFire",EOC);
            // this->AddCoupling(plt2[i],"PositionOut",this,"RedPosition",EOC);
            // this->AddCoupling(this,"BluePosition",plt2[i],"PositionIn",EIC);
        }
        this->LogMyBirth();
    }
};