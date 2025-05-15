#pragma once
#include "atomic_model.hpp" 

class TestCannon : public AtomicModel{
public:
    TestCannon(Engine* engine){
        this->SetEngine(engine);
    }
};