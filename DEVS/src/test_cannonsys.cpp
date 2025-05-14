#pragma once
#include "coupled_model.hpp"
#include "test_world.cpp"

class Engine;

class TestCannonSys : public CoupledModel{
    
    TestCannonSys(int id, Engine* engine){
        this->SetModelID(id);
        this->SetEngine(engine);
        TestWorld();        
    }
};