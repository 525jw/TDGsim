#pragma once
#include "DEVS/coupled_model.hpp"
#include "DEVS/logger.hpp"
#include "EF/experiment_frame.hpp"
#include "SIM/simulation.hpp"

class TdgSim : public CoupledModel{
public:
    TdgSim(Engine* engine)
    : CoupledModel(engine)
    {
        this->SetModelName("TDGSim");
        
        ExperimentFrame* ef = new ExperimentFrame(engine);
        Simulation* sim = new Simulation(engine);

        ef->SetModelName("EF");
        ef->SetParentModel(this);
        this->RegisterSubModel(ef);
        
        sim->SetModelName("SIM");
        sim->SetParentModel(this);
        this->RegisterSubModel(sim);

        this->AddCoupling(ef,"Start",sim,"Start",IC);
        this->AddCoupling(sim,"Result",ef,"Result",IC);
    }
};