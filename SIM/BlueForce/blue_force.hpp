#pragma once
#include "DEVS/coupled_model.hpp"
#include "DEVS/logger.hpp"
#include "hq.hpp"
#include "BlueInfantry/blue_infantry.hpp"
// #include "BlueArmor/blue_armor.hpp"
// #include "BlueArtillery/blue_artillery.hpp"

class BlueForce : public CoupledModel{
public:
    BlueForce(Engine* engine)
    : CoupledModel(engine)
    {
        Environment* environment = new Environment(engine);
        this->RegisterModelWithID(environment);
        environment->SetParentModel(this);

        BlueInfantry* blueInfantry = new BlueInfantry(engine);
        this->RegisterModelWithID(blueInfantry);
        blueInfantry->SetParentModel(this);
        
        this->AddInputPort("RedFire");
        this->AddOutputPort("BluePosition");

        this->AddCoupling(this,"RedFire",blueInfantry,"FireIn",EIC);
        this->AddCoupling(blueInfantry,"MnvRes",this,"BluePosition",EOC);
        this->AddCoupling(blueInfantry,"FireRes",this,"BlueFire",EOC);
    }
};