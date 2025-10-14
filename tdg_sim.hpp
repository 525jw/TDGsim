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
        // 생성-engine정보만 넘겨주기
        ExperimentFrame* ef = new ExperimentFrame(engine);
        Simulation* sim = new Simulation(engine);

        
        ef->SetParentModel(this);
        sim->SetParentModel(this);
        this->RegisterSubModel(ef);
        this->RegisterSubModel(sim);

        this->AddCoupling(ef,"Start",sim,"Start",IC);
        this->AddCoupling(sim,"Result",ef,"Result",IC);
    }
};