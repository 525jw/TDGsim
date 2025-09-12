#pragma once
#include "DEVS/coupled_model.hpp"
#include "DEVS/logger.hpp"
#include "SIM/Environment/environment.hpp"
#include "BlueForce/blue_force.hpp"
#include "RedForce/red_force.hpp"

class Simulation : public CoupledModel{
public:
    Simulation(Engine* engine)
    : CoupledModel(engine)
    {
        Environment* environment = new Environment(engine);
        BlueForce* blueForce = new BlueForce(engine);
        RedForce* redForce = new RedForce(engine);
        this->engine->RegisterModelInEngine(environment);
        this->engine->RegisterModelInEngine(blueForce);
        this->engine->RegisterModelInEngine(redForce);
        this->RegisterSubModel(environment);
        this->RegisterSubModel(blueForce);
        this->RegisterSubModel(redForce);
        environment->SetParentModel(this);
        blueForce->SetParentModel(this);
        redForce->SetParentModel(this);

        this->AddInputPort("Start");
        this->AddOutputPort("Result");

        this->AddCoupling(this,"Start",environment,"Start",EIC);
        this->AddCoupling(this,"Start",blueForce,"Start",EIC);
        this->AddCoupling(blueForce,"BluePosition",redForce,"BluePosition",IC);
        this->AddCoupling(redForce,"RedPosition",blueForce,"RedPosition",IC);
        this->AddCoupling(blueForce,"BlueFire",redForce,"BlueFire",IC);
        this->AddCoupling(redForce,"RedFire",blueForce,"RedFire",IC);
    }
};