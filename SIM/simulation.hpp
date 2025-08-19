#pragma once
#include "DEVS/coupled_model.hpp"
#include "DEVS/logger.hpp"
#include "environment.hpp"
#include "BlueForce/blue_force.hpp"
// #include "RedForce/red_force.hpp"

class Simulation : public CoupledModel{
public:
    Simulation(Engine* engine)
    : CoupledModel(engine)
    {
        Environment* environment = new Environment(engine);
        this->RegisterModelWithID(environment);
        environment->SetParentModel(this);

        BlueForce* blueForce = new BlueForce(engine);
        this->RegisterModelWithID(blueForce);
        blueForce->SetParentModel(this);
        
        this->AddInputPort("start_");
        this->AddOutputPort("result_");

        this->AddCoupling(this,"start_",environment,"ScenInfo",EIC);
    }
};