#pragma once
#include "DEVS/coupled_model.hpp"
#include "DEVS/logger.hpp"
#include "generator.hpp"
// #include "data_collector.hpp"

class ExperimentFrame : public CoupledModel{
public:
    ExperimentFrame(Engine* engine)
    : CoupledModel(engine)
    {
        Generator* generator = new Generator(engine);
        this->RegisterModelWithID(generator);
        generator->SetParentModel(this);

        // DataCollector* dataCollector = new DataCollector(engine);
        // this->RegisterModelWithID(dataCollector);
        // dataCollector->SetParentModel(this);
        
        this->AddInputPort("result");
        this->AddOutputPort("start");

        this->AddCoupling(generator,"start",this,"start",EOC);
        // this->AddCoupling(this,"result",dataCollector,"result",EIC);
        // this->AddCoupling(dataCollector,"restart",generator,"restart",IC);
    }
};