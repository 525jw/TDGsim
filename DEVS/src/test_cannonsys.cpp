#pragma once
#include "coupled_model.hpp"
#include "test_world.cpp"

class Engine;

class TestCannonSys : public CoupledModel{
    
    TestCannonSys(Engine* engine){
        this->SetEngine(engine);
        TestWorld();        
    }
};