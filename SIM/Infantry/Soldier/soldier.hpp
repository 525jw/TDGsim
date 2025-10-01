#pragma once
#include "DEVS/coupled_model.hpp"
#include "maneuver.hpp"
#include "fire.hpp"
#include "detection.hpp"
#include "DEVS/logger.hpp"

class Engine;

class Soldier : public CoupledModel {
private:
    int entityId;
    Entity info;
public:
    Soldier(Engine* engine, int entityId, Entity info)
        : CoupledModel(engine)
    {
        this->entityId=entityId;
        this->info=info;
        
        // 생성
        Maneuver* mnv = new Maneuver(engine, this->entityId, &this->info);
        Detection* det = new Detection(engine, this->entityId, &this->info);
        Fire* fire = new Fire(engine, this->entityId, &this->info);

        // 부모/자식모델 연결
        this->RegisterSubModel(mnv); mnv->SetParentModel(this);
        this->RegisterSubModel(det); det->SetParentModel(this);
        this->RegisterSubModel(fire); fire->SetParentModel(this);

        this->AddInputPort("PlatoonOrd"); // mnv in
        this->AddInputPort("PositionIn"); // det in
        this->AddInputPort("FireIn"); // fire in
        this->AddOutputPort("PositionOut"); // mnv out
        this->AddOutputPort("SoldierRep"); // det out
        this->AddOutputPort("FireOut"); // fire out

        this->AddCoupling(this,"PlatoonOrd",mnv,"PlatoonOrd",EIC);
        this->AddCoupling(this,"PositionIn",det,"PositionIn",EIC);
        this->AddCoupling(this,"FireIn",mnv,"FireIn",EIC);
        this->AddCoupling(mnv,"PositionOut",det,"PositionOut",IC);
        this->AddCoupling(det,"SoldierRep",fire,"SoldierRep",IC);
        this->AddCoupling(mnv,"PositionOut",this,"PositionOut",EOC);
        this->AddCoupling(det,"SoldierRep",this,"SoldierRep",EOC);
        this->AddCoupling(fire,"FireOut",this,"FireOut",EOC);

    }
};
