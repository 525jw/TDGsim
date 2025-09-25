#pragma once
#include "DEVS/coupled_model.hpp"
#include "maneuver.hpp"
#include "fire.hpp"
#include "detection.hpp"
#include "DEVS/logger.hpp"

class Engine;

class BlueSoldier : public CoupledModel {
private:
    int entityId;
    Entity info;
public:
    BlueSoldier(Engine* engine, int entityId, Entity info)
        : CoupledModel(engine)
    {
        this->entityId=entityId;
        this->info=info;
        
        // 생성
        Maneuver* mnv = new Maneuver(engine, this->entityId, &this->info);
        Detection* det = new Detection(engine, this->entityId, &this->info);
        Fire* fire = new Fire(engine, this->entityId, &this->info);
        // engine에 등록
        this->engine->RegisterModelInEngine(mnv);
        this->engine->RegisterModelInEngine(det);
        this->engine->RegisterModelInEngine(fire);
        // 자식모델 목록에 등록
        this->RegisterSubModel(mnv);
        this->RegisterSubModel(det);
        this->RegisterSubModel(fire);
        // 부모를 this로 설정
        mnv->SetParentModel(this);
        det->SetParentModel(this);
        fire->SetParentModel(this);
        this->AddInputPort("PlatoonOrd");
        this->AddInputPort("PositionIn");
        this->AddInputPort("FireIn");
        this->AddOutputPort("SoldierRep");
        this->AddOutputPort("PositionOut");
        this->AddOutputPort("FireOut");

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
