#pragma once
#include "DEVS/coupled_model.hpp"
#include "DEVS/logger.hpp"
#include "BlueHQ/blue_hq.hpp"
#include "BlueInfantry/blue_infantry.hpp"
// #include "BlueArmor/blue_armor.hpp"
// #include "BlueArtillery/blue_artillery.hpp"

class BlueForce : public CoupledModel{
public:
    BlueForce(Engine* engine)
    : CoupledModel(engine)
    {
        BlueHQ* blueHQ = new BlueHQ(engine);
        BlueInfantry* blueInfantry = new BlueInfantry(engine);

        this->engine->RegisterModelInEngine(blueHQ);
        this->engine->RegisterModelInEngine(blueInfantry);

        this->RegisterSubModel(blueHQ);
        this->RegisterSubModel(blueInfantry);
        blueHQ->SetParentModel(this);
        blueInfantry->SetParentModel(this);

        this->AddInputPort("Start");

        this->AddInputPort("RedFire");
        this->AddInputPort("RedPosition");
        this->AddOutputPort("BlueFire");
        this->AddOutputPort("BluePosition");

        this->AddCoupling(this,"Start",blueHQ,"Start",EIC);
        this->AddCoupling(blueHQ,"CompanyOrd",blueInfantry,"CompanyOrd",IC);
        this->AddCoupling(blueInfantry,"InfantryRep",blueHQ,"InfantryRep",IC);

        this->AddCoupling(this,"RedFire",blueInfantry,"FireIn",EIC);
        this->AddCoupling(this,"RedPosition",blueInfantry,"PositionIn",EIC);
        this->AddCoupling(blueInfantry,"PositionOut",this,"BluePosition",EOC);
        this->AddCoupling(blueInfantry,"FireOut",this,"BlueFire",EOC);
    }
};