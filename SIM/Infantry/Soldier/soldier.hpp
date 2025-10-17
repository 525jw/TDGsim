#pragma once
#include "DEVS/coupled_model.hpp"
#include "maneuver.hpp"
#include "fire.hpp"
#include "detection.hpp"
#include "DEVS/logger.hpp"

class Engine;

class Soldier : public CoupledModel {
private:
    Entity info;
public:
    Soldier(Engine* engine, Entity info)
        : CoupledModel(engine)
    {
        this->info=info;
        this->SetModelName(info.name);
        
        // 생성
        Maneuver* mnv = new Maneuver(engine, &this->info);
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
        // this->AddInputPort("PositionIn");
        this->AddInputPort("FireIn");
        this->AddOutputPort("PositionOut");
        this->AddOutputPort("SoldierRep");
        this->AddOutputPort("FireOut");

        this->AddCoupling(this,"PlatoonOrd",mnv,"Order",EIC);
        // this->AddCoupling(this,"PositionIn",det,"EnemyPosition",EIC);
        this->AddCoupling(this,"FireIn",mnv,"FireIn",EIC);
        this->AddCoupling(mnv,"PositionOut",det,"MyPosition",IC);
        this->AddCoupling(det,"SoldierRep",fire,"SoldierRep",IC);
        // this->AddCoupling(mnv,"PositionOut",this,"PositionOut",EOC);
        this->AddCoupling(det,"SoldierRep",this,"SoldierRep",EOC);
        this->AddCoupling(fire,"FireOut",this,"FireOut",EOC);
    }
};
