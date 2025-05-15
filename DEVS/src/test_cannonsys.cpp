#pragma once
#include "coupled_model.hpp"
#include "test_cannonteam.cpp"

class Engine;

class TestCannonSys : public CoupledModel{
public:
    TestCannonSys(Engine* engine)
    : CoupledModel(engine)
    {
        std::cout<<"! cannon system created\n";
        TestCannonTeam* blueteam = new TestCannonTeam(engine,"blue");
        TestCannonTeam* redteam = new TestCannonTeam(engine,"red");

        this->AddComponent(blueteam);
        this->AddComponent(redteam);

        this->AddCoupling(blueteam, "fire_out", redteam, "fire_in", IC);
        this->AddCoupling(redteam, "fire_out", blueteam, "fire_in", IC);
    }
};