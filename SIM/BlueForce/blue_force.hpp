#pragma once
#include "DEVS/coupled_model.hpp"
#include "common.hpp"
#include "DEVS/logger.hpp"
#include "SIM/Environment/environment.hpp"
#include "blue_cgf.hpp"

class BlueForce : public CoupledModel{
public:
    BlueForce(Engine* engine)
    : CoupledModel(engine)
    {
        // === 생성 ===
        std::vector<BlueCgf*> plt1;
        const int numOfPl1 = 25;
        plt1.reserve(numOfPl1);

        std::vector<BlueCgf*> plt2;
        const int numOfPl2 = 25;
        plt2.reserve(numOfPl2);

        // === BLUE PLT1 ===
        plt1.push_back(new BlueCgf(engine, env->RegisterEntityIdByName("BLUE-PLT1-SOL01"), Entity{"BLUE-PLT1-SOL01", Side::BLUE, ForceType::RIFLE, {14,68}}));
        plt1.push_back(new BlueCgf(engine, env->RegisterEntityIdByName("BLUE-PLT1-SOL02"), Entity{"BLUE-PLT1-SOL02", Side::BLUE, ForceType::RIFLE, {13,67}}));
        plt1.push_back(new BlueCgf(engine, env->RegisterEntityIdByName("BLUE-PLT1-SOL03"), Entity{"BLUE-PLT1-SOL03", Side::BLUE, ForceType::RIFLE, {13,68}}));
        plt1.push_back(new BlueCgf(engine, env->RegisterEntityIdByName("BLUE-PLT1-SOL04"), Entity{"BLUE-PLT1-SOL04", Side::BLUE, ForceType::RIFLE, {13,69}}));
        plt1.push_back(new BlueCgf(engine, env->RegisterEntityIdByName("BLUE-PLT1-SOL05"), Entity{"BLUE-PLT1-SOL05", Side::BLUE, ForceType::RIFLE, {14,67}}));
        plt1.push_back(new BlueCgf(engine, env->RegisterEntityIdByName("BLUE-PLT1-SOL06"), Entity{"BLUE-PLT1-SOL06", Side::BLUE, ForceType::RIFLE, {14,69}}));
        plt1.push_back(new BlueCgf(engine, env->RegisterEntityIdByName("BLUE-PLT1-SOL07"), Entity{"BLUE-PLT1-SOL07", Side::BLUE, ForceType::RIFLE, {15,67}}));
        plt1.push_back(new BlueCgf(engine, env->RegisterEntityIdByName("BLUE-PLT1-SOL08"), Entity{"BLUE-PLT1-SOL08", Side::BLUE, ForceType::RIFLE, {15,68}}));
        plt1.push_back(new BlueCgf(engine, env->RegisterEntityIdByName("BLUE-PLT1-SOL09"), Entity{"BLUE-PLT1-SOL09", Side::BLUE, ForceType::RIFLE, {15,69}}));
        plt1.push_back(new BlueCgf(engine, env->RegisterEntityIdByName("BLUE-PLT1-SOL10"), Entity{"BLUE-PLT1-SOL10", Side::BLUE, ForceType::RIFLE, {12,66}}));
        plt1.push_back(new BlueCgf(engine, env->RegisterEntityIdByName("BLUE-PLT1-SOL11"), Entity{"BLUE-PLT1-SOL11", Side::BLUE, ForceType::RIFLE, {12,67}}));
        plt1.push_back(new BlueCgf(engine, env->RegisterEntityIdByName("BLUE-PLT1-SOL12"), Entity{"BLUE-PLT1-SOL12", Side::BLUE, ForceType::RIFLE, {12,68}}));
        plt1.push_back(new BlueCgf(engine, env->RegisterEntityIdByName("BLUE-PLT1-SOL13"), Entity{"BLUE-PLT1-SOL13", Side::BLUE, ForceType::RIFLE, {13,66}}));
        plt1.push_back(new BlueCgf(engine, env->RegisterEntityIdByName("BLUE-PLT1-SOL14"), Entity{"BLUE-PLT1-SOL14", Side::BLUE, ForceType::RIFLE, {14,66}}));
        plt1.push_back(new BlueCgf(engine, env->RegisterEntityIdByName("BLUE-PLT1-SOL15"), Entity{"BLUE-PLT1-SOL15", Side::BLUE, ForceType::RIFLE, {12,69}}));
        plt1.push_back(new BlueCgf(engine, env->RegisterEntityIdByName("BLUE-PLT1-SOL16"), Entity{"BLUE-PLT1-SOL16", Side::BLUE, ForceType::RIFLE, {12,70}}));
        plt1.push_back(new BlueCgf(engine, env->RegisterEntityIdByName("BLUE-PLT1-SOL17"), Entity{"BLUE-PLT1-SOL17", Side::BLUE, ForceType::RIFLE, {13,70}}));
        plt1.push_back(new BlueCgf(engine, env->RegisterEntityIdByName("BLUE-PLT1-SOL18"), Entity{"BLUE-PLT1-SOL18", Side::BLUE, ForceType::RIFLE, {14,70}}));
        plt1.push_back(new BlueCgf(engine, env->RegisterEntityIdByName("BLUE-PLT1-SOL19"), Entity{"BLUE-PLT1-SOL19", Side::BLUE, ForceType::RIFLE, {15,66}}));
        plt1.push_back(new BlueCgf(engine, env->RegisterEntityIdByName("BLUE-PLT1-SOL20"), Entity{"BLUE-PLT1-SOL20", Side::BLUE, ForceType::RIFLE, {16,66}}));
        plt1.push_back(new BlueCgf(engine, env->RegisterEntityIdByName("BLUE-PLT1-SOL21"), Entity{"BLUE-PLT1-SOL21", Side::BLUE, ForceType::RIFLE, {16,67}}));
        plt1.push_back(new BlueCgf(engine, env->RegisterEntityIdByName("BLUE-PLT1-SOL22"), Entity{"BLUE-PLT1-SOL22", Side::BLUE, ForceType::RIFLE, {16,68}}));
        plt1.push_back(new BlueCgf(engine, env->RegisterEntityIdByName("BLUE-PLT1-SOL23"), Entity{"BLUE-PLT1-SOL23", Side::BLUE, ForceType::RIFLE, {16,69}}));
        plt1.push_back(new BlueCgf(engine, env->RegisterEntityIdByName("BLUE-PLT1-SOL24"), Entity{"BLUE-PLT1-SOL24", Side::BLUE, ForceType::RIFLE, {16,70}}));
        plt1.push_back(new BlueCgf(engine, env->RegisterEntityIdByName("BLUE-PLT1-SOL25"), Entity{"BLUE-PLT1-SOL25", Side::BLUE, ForceType::RIFLE, {17,67}}));

        // === BLUE PLT2 ===
        plt2.push_back(new BlueCgf(engine, env->RegisterEntityIdByName("BLUE-PLT2-SOL01"), Entity{"BLUE-PLT2-SOL01", Side::BLUE, ForceType::RIFLE, {82,68}}));
        plt2.push_back(new BlueCgf(engine, env->RegisterEntityIdByName("BLUE-PLT2-SOL02"), Entity{"BLUE-PLT2-SOL02", Side::BLUE, ForceType::RIFLE, {81,67}}));
        plt2.push_back(new BlueCgf(engine, env->RegisterEntityIdByName("BLUE-PLT2-SOL03"), Entity{"BLUE-PLT2-SOL03", Side::BLUE, ForceType::RIFLE, {81,68}}));
        plt2.push_back(new BlueCgf(engine, env->RegisterEntityIdByName("BLUE-PLT2-SOL04"), Entity{"BLUE-PLT2-SOL04", Side::BLUE, ForceType::RIFLE, {81,69}}));
        plt2.push_back(new BlueCgf(engine, env->RegisterEntityIdByName("BLUE-PLT2-SOL05"), Entity{"BLUE-PLT2-SOL05", Side::BLUE, ForceType::RIFLE, {82,67}}));
        plt2.push_back(new BlueCgf(engine, env->RegisterEntityIdByName("BLUE-PLT2-SOL06"), Entity{"BLUE-PLT2-SOL06", Side::BLUE, ForceType::RIFLE, {82,69}}));
        plt2.push_back(new BlueCgf(engine, env->RegisterEntityIdByName("BLUE-PLT2-SOL07"), Entity{"BLUE-PLT2-SOL07", Side::BLUE, ForceType::RIFLE, {83,67}}));
        plt2.push_back(new BlueCgf(engine, env->RegisterEntityIdByName("BLUE-PLT2-SOL08"), Entity{"BLUE-PLT2-SOL08", Side::BLUE, ForceType::RIFLE, {83,68}}));
        plt2.push_back(new BlueCgf(engine, env->RegisterEntityIdByName("BLUE-PLT2-SOL09"), Entity{"BLUE-PLT2-SOL09", Side::BLUE, ForceType::RIFLE, {83,69}}));
        plt2.push_back(new BlueCgf(engine, env->RegisterEntityIdByName("BLUE-PLT2-SOL10"), Entity{"BLUE-PLT2-SOL10", Side::BLUE, ForceType::RIFLE, {80,66}}));
        plt2.push_back(new BlueCgf(engine, env->RegisterEntityIdByName("BLUE-PLT2-SOL11"), Entity{"BLUE-PLT2-SOL11", Side::BLUE, ForceType::RIFLE, {80,67}}));
        plt2.push_back(new BlueCgf(engine, env->RegisterEntityIdByName("BLUE-PLT2-SOL12"), Entity{"BLUE-PLT2-SOL12", Side::BLUE, ForceType::RIFLE, {80,68}}));
        plt2.push_back(new BlueCgf(engine, env->RegisterEntityIdByName("BLUE-PLT2-SOL13"), Entity{"BLUE-PLT2-SOL13", Side::BLUE, ForceType::RIFLE, {81,66}}));
        plt2.push_back(new BlueCgf(engine, env->RegisterEntityIdByName("BLUE-PLT2-SOL14"), Entity{"BLUE-PLT2-SOL14", Side::BLUE, ForceType::RIFLE, {82,66}}));
        plt2.push_back(new BlueCgf(engine, env->RegisterEntityIdByName("BLUE-PLT2-SOL15"), Entity{"BLUE-PLT2-SOL15", Side::BLUE, ForceType::RIFLE, {80,69}}));
        plt2.push_back(new BlueCgf(engine, env->RegisterEntityIdByName("BLUE-PLT2-SOL16"), Entity{"BLUE-PLT2-SOL16", Side::BLUE, ForceType::RIFLE, {80,70}}));
        plt2.push_back(new BlueCgf(engine, env->RegisterEntityIdByName("BLUE-PLT2-SOL17"), Entity{"BLUE-PLT2-SOL17", Side::BLUE, ForceType::RIFLE, {81,70}}));
        plt2.push_back(new BlueCgf(engine, env->RegisterEntityIdByName("BLUE-PLT2-SOL18"), Entity{"BLUE-PLT2-SOL18", Side::BLUE, ForceType::RIFLE, {82,70}}));
        plt2.push_back(new BlueCgf(engine, env->RegisterEntityIdByName("BLUE-PLT2-SOL19"), Entity{"BLUE-PLT2-SOL19", Side::BLUE, ForceType::RIFLE, {83,66}}));
        plt2.push_back(new BlueCgf(engine, env->RegisterEntityIdByName("BLUE-PLT2-SOL20"), Entity{"BLUE-PLT2-SOL20", Side::BLUE, ForceType::RIFLE, {84,66}}));
        plt2.push_back(new BlueCgf(engine, env->RegisterEntityIdByName("BLUE-PLT2-SOL21"), Entity{"BLUE-PLT2-SOL21", Side::BLUE, ForceType::RIFLE, {84,67}}));
        plt2.push_back(new BlueCgf(engine, env->RegisterEntityIdByName("BLUE-PLT2-SOL22"), Entity{"BLUE-PLT2-SOL22", Side::BLUE, ForceType::RIFLE, {84,68}}));
        plt2.push_back(new BlueCgf(engine, env->RegisterEntityIdByName("BLUE-PLT2-SOL23"), Entity{"BLUE-PLT2-SOL23", Side::BLUE, ForceType::RIFLE, {84,69}}));
        plt2.push_back(new BlueCgf(engine, env->RegisterEntityIdByName("BLUE-PLT2-SOL24"), Entity{"BLUE-PLT2-SOL24", Side::BLUE, ForceType::RIFLE, {84,70}}));
        plt2.push_back(new BlueCgf(engine, env->RegisterEntityIdByName("BLUE-PLT2-SOL25"), Entity{"BLUE-PLT2-SOL25", Side::BLUE, ForceType::RIFLE, {85,67}}));

        for (int i = 0; i < numOfPl1; ++i) {
            this->RegisterSubModel(plt1[i]);
            plt1[i]->SetParentModel(this);
        }
        for (int i = 0; i < numOfPl2; ++i) {
            this->RegisterSubModel(plt2[i]);
            plt2[i]->SetParentModel(this);
        }

        this->AddInputPort("Start");
        this->AddInputPort("RedFire");
        this->AddOutputPort("BlueFire");
        // this->AddInputPort("RedPosition");
        // this->AddOutputPort("BluePosition");

        // === 커플링 ===
        for (int i = 0; i < numOfPl1; ++i) {
            this->AddCoupling(this, "Start", plt1[i], "Start", EIC);
            this->AddCoupling(this, "RedFire",     plt1[i], "FireIn",     EIC);
            this->AddCoupling(plt1[i], "FireOut",      this, "BlueFire",     EOC);
            // this->AddCoupling(this,"RedPosition",plt1[i],"PositionIn",EIC);
            // this->AddCoupling(plt1[i], "PositionOut", this, "BluePosition", EOC);
        }
        for (int i = 0; i < numOfPl2; ++i) {
            this->AddCoupling(this, "Start", plt2[i], "Start", EIC);
            this->AddCoupling(this, "RedFire",     plt2[i], "FireIn",     EIC);
            this->AddCoupling(plt2[i], "FireOut",      this, "BlueFire",     EOC);
            // this->AddCoupling(plt2[i], "PositionOut", this, "BluePosition", EOC);
            // this->AddCoupling(this,"RedPosition",plt2[i],"PositionIn",EIC);
        }
        this->LogMyBirth();
    }
};