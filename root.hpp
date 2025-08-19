#pragma once
#include "DEVS/coupled_model.hpp"
#include "DEVS/logger.hpp"
#include "EF/experiment_frame.hpp"
#include "SIM/simulation.hpp"

class Root : public CoupledModel{
public:
    Root(Engine* engine)
    : CoupledModel(engine)
    {
        ExperimentFrame* experimentFrame = new ExperimentFrame(engine);
        this->RegisterModelWithID(experimentFrame);
        experimentFrame->SetParentModel(this);

        Simulation* simulation = new Simulation(engine);
        this->RegisterModelWithID(simulation);
        simulation->SetParentModel(this);

        this->AddCoupling(experimentFrame,"start_",simulation,"start_",IC);
        this->AddCoupling(simulation,"result_",experimentFrame,"result_",IC);
    }
};