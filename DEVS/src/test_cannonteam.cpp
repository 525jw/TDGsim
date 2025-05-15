#pragma once
#include "coupled_model.hpp"
#include "test_cannon.cpp"
#include <string>

class TestCannonTeam : public CoupledModel{
private:
    std::string myName;
public:
    TestCannonTeam(Engine* engine,std::string name)
    : CoupledModel(engine)
    {
        this->myName=name;
        std::cout<<"! ! "<<myName<<" created\n";
        TestCannon* firstCannon = new TestCannon(engine,this->myName + "first");
        TestCannon* secondCannon = new TestCannon(engine,this->myName + "second");
        TestCannon* thirdCannon = new TestCannon(engine,this->myName + "third");
        
        this->AddComponent(firstCannon);
        this->AddComponent(secondCannon);
        this->AddComponent(thirdCannon);

        this->AddInputPort("fire_in");
        this->AddOutputPort("fire_out");

        this->AddCoupling(firstCannon,"fire_out",this,"fire_out",EOC);
        this->AddCoupling(this,"fire_in",firstCannon,"fire_in",EIC);

        this->AddCoupling(secondCannon,"fire_out",this,"fire_out",EOC);
        this->AddCoupling(this,"fire_in",secondCannon,"fire_in",EIC);

        this->AddCoupling(thirdCannon,"fire_out",this,"fire_out",EOC);
        this->AddCoupling(this,"fire_in",thirdCannon,"fire_in",EIC);

    }
};