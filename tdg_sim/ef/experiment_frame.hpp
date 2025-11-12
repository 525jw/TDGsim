#pragma once
#include "DEVS/coupled_model.hpp"
#include "DEVS/logger.hpp"
#include "generator.hpp"
#include "transducer.hpp"

class ExperimentFrame : public CoupledModel{
public:
    ExperimentFrame(Engine* engine)
    : CoupledModel(engine)
    {
        // 생성
        Generator* generator = new Generator(engine);
        Transducer* transducer = new Transducer(engine);


        // 이름 설정-id는 생성자에서 자동 등록,TODO 이름도 생성자에서 handle가능하게
        generator->SetModelName("Generator");
        transducer->SetModelName("Transducer");
        // 부모를 this로 설정
        generator->SetParentModel(this);
        transducer->SetParentModel(this);   
        // 자식모델 목록에 등록
        this->RegisterSubModel(generator);
        this->RegisterSubModel(transducer);

        // IO port 설정
        this->AddInputPort("Result");
        this->AddOutputPort("Start");
        
        // Coupling
        this->AddCoupling(generator,"Start",this,"Start",EOC);
        this->AddCoupling(generator,"Start",transducer,"Start",IC);
        this->AddCoupling(transducer,"Restart",generator,"Restart",IC);
    }
};