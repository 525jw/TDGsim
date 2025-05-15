#pragma once
#include "coupled_model.hpp"
#include "test_cannon.cpp"

class TestCannonTeam : public CoupledModel{
public:
    TestCannonTeam(Engine* engine){
        this->SetEngine(engine);
        TestCannon* firstCannon = new TestCannon(engine);
        TestCannon* secondCannon = new TestCannon(engine);
        TestCannon* thirdCannon = new TestCannon(engine);
        
        this->AddComponent(firstCannon);
        this->AddComponent(secondCannon);
        this->AddComponent(thirdCannon);
    }
};