#pragma once
#include "DEVS/coupled_model.hpp"
#include "DEVS/logger.hpp"

class IamCM : public CoupledModel{
public:
    IamCM(Engine* engine)
    : CoupledModel(engine)
    {
        // 생성
        // ex) SubM* subM = new subM(engine);

        // engine에 등록
        // ex) this->engine->RegisterModelInEngine(subM);

        // 자식모델 목록에 등록
        // ex) this->RegisterSubModel(subM);

        // 부모를 this로 설정
        // ex) subM->SetParentModel(this);

        // IO port 설정
        // ex) this->AddInputPort("SubMIn");
        // ex) this->AddOutputPort("SubMOut");

        // Coupling
        // ex) this->AddCoupling(subM,"SubMOut",this,"SubMOut",EOC);
        // ex) this->AddCoupling(this,"SubMIn",subM,"SubMIn",EIC);
    }
};