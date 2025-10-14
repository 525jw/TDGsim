#pragma once
#include "DEVS/coupled_model.hpp"
#include "DEVS/logger.hpp"
#include "SIM/Environment/environment.hpp"
#include "blue_force.hpp"
#include "red_force.hpp"

class Simulation : public CoupledModel{
public:
    Simulation(Engine* engine)
    : CoupledModel(engine)
    {   
        env = new Environment(engine);
        BlueForce* blueForce = new BlueForce(engine);
        RedForce* redForce = new RedForce(engine);
        
        this->RegisterSubModel(env);
        this->RegisterSubModel(blueForce);
        this->RegisterSubModel(redForce);
        env->SetParentModel(this);
        blueForce->SetParentModel(this);
        redForce->SetParentModel(this);

        this->AddInputPort("Start");
        this->AddOutputPort("Result");

        this->AddCoupling(this,"Start",env,"Start",EIC);
        this->AddCoupling(this,"Start",blueForce,"Start",EIC);
        this->AddCoupling(this,"Start",redForce,"Start",EIC);
        this->AddCoupling(blueForce,"BlueFire",redForce,"BlueFire",IC);
        this->AddCoupling(redForce,"RedFire",blueForce,"RedFire",IC);
        this->AddCoupling(blueForce,"BluePosition",redForce,"BluePosition",IC);
        this->AddCoupling(redForce,"RedPosition",blueForce,"RedPosition",IC);
    }
};