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

        // 이름 설정-id는 생성자에서 자동 등록,TODO 이름도 생성자에서 handle가능하게
        generator->SetModelName("Generator");
        // 부모를 this로 설정
        generator->SetParentModel(this);
        // 자식모델 목록에 등록
        this->RegisterSubModel(generator);

        // IO port 설정
        this->AddInputPort("Result");
        this->AddOutputPort("Start");
        
        // Coupling
        this->AddCoupling(generator,"Start",this,"Start",EOC);
        // this->AddCoupling(this,"Result",dataCollector,"Result",EIC);
        // this->AddCoupling(dataCollector,"Restart",generator,"Restart",IC);
    }
};