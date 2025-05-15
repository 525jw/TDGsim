#pragma once
#include "coupled_model.hpp"
#include "test_cannonteam.cpp"

class Engine;

class TestCannonSys : public CoupledModel{
public:
    TestCannonSys(Engine* engine){
        this->SetEngine(engine);
        
        TestCannonTeam* blueteam = new TestCannonTeam(engine);
        TestCannonTeam* redteam = new TestCannonTeam(engine);

        this->AddComponent(blueteam);
        this->AddComponent(redteam);
    }
};