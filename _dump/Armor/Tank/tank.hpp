#pragma once
#include "DEVS/coupled_model.hpp"
#include "common.hpp"
#include "DEVS/logger.hpp"
#include "SIM/Environment/environment.hpp"

// Reuse infantry Detection and Fire logic as-is
#include "SIM/Infantry/Soldier/detection.hpp"
#include "SIM/Infantry/Soldier/fire.hpp"

#include "maneuver.hpp"

class Engine;

class Tank : public CoupledModel {
private:
    Entity info;
public:
    Tank(Engine* engine, Entity info)
        : CoupledModel(engine)
    {
        this->info=info;
        this->SetModelName(info.name);

        // 구성
        TankManeuver* mnv = new TankManeuver(engine, &this->info);
        Detection* det = new Detection(engine, &this->info);
        Fire* fire = new Fire(engine, &this->info);

        mnv->SetModelName(info.name+"-MNV");
        mnv->SetParentModel(this);
        this->RegisterSubModel(mnv);

        det->SetModelName(info.name+"-DET");
        det->SetParentModel(this);
        this->RegisterSubModel(det);

        fire->SetModelName(info.name+"-FIR");
        fire->SetParentModel(this);
        this->RegisterSubModel(fire);

        this->AddInputPort("PlatoonOrd");
        this->AddInputPort("FireIn");
        this->AddOutputPort("PositionOut");
        this->AddOutputPort("SoldierRep");
        this->AddOutputPort("FireOut");

        this->AddCoupling(this,"PlatoonOrd",mnv,"Order",EIC);
        this->AddCoupling(this,"FireIn",mnv,"FireIn",EIC);
        this->AddCoupling(mnv,"PositionOut",det,"MyPosition",IC);
        this->AddCoupling(det,"SoldierRep",fire,"SoldierRep",IC);
        this->AddCoupling(det,"SoldierRep",this,"SoldierRep",EOC);
        this->AddCoupling(fire,"FireOut",this,"FireOut",EOC);
    }
};

