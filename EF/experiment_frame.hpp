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
        // 생성
        Generator* generator = new Generator(engine);
        // DataCollector* dataCollector = new DataCollector(engine);

        // 자식모델 목록에 등록
        this->RegisterSubModel(generator);
        // this->RegisterSubModel(dataCollector);

        // 부모를 this로 설정
        generator->SetParentModel(this);
        // dataCollector->SetParentModel(this);

        // IO port 설정
        this->AddInputPort("Result");
        this->AddOutputPort("Start");
        
        // Coupling
        this->AddCoupling(generator,"Start",this,"Start",EOC);
        // this->AddCoupling(this,"Result",dataCollector,"Result",EIC);
        // this->AddCoupling(dataCollector,"Restart",generator,"Restart",IC);
    }
};