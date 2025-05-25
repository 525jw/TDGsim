#pragma once
#include "coupled_model.hpp"
#include "test_cannon.cpp"
#include "logger.hpp"
#include <string>

class TestCannonTeam : public CoupledModel{
private:
    std::string myName;
public:
    TestCannonTeam(int modelID, Engine* engine, std::string name)
    : CoupledModel(modelID, engine)
    {
        this->myName=name;
        logger <<"[Model : Init]" <<"! ! "<<myName<<" created id:"<<this->GetModelID()<<"\n";
        int idNum = this->GetModelID()*10;
        
        TestCannon* firstCannon = new TestCannon(idNum+1, engine,this->myName + "first");
        firstCannon->SetParentModel(this);
        TestCannon* secondCannon = new TestCannon(idNum+2, engine,this->myName + "second");
        secondCannon->SetParentModel(this);
        TestCannon* thirdCannon = new TestCannon(idNum+3, engine,this->myName + "third");
        thirdCannon->SetParentModel(this);
        
        this->RegisterModelWithID(firstCannon);
        this->RegisterModelWithID(secondCannon);
        this->RegisterModelWithID(thirdCannon);

        this->AddInputPort("fire_in");
        this->AddOutputPort("fire_out");

        this->AddCoupling(firstCannon,"fire_out",this,"fire_out",EOC);
        this->AddCoupling(this,"fire_in",firstCannon,"fire_in",EIC);

        this->AddCoupling(secondCannon,"fire_out",this,"fire_out",EOC);
        this->AddCoupling(this,"fire_in",secondCannon,"fire_in",EIC);

        this->AddCoupling(thirdCannon,"fire_out",this,"fire_out",EOC);
        this->AddCoupling(this,"fire_in",thirdCannon,"fire_in",EIC);

        std::vector<int> keys;
        for (const auto& pair : this->modelsWithID) {
            keys.push_back(pair.first);
        }
        for (const auto& key : keys) {
            logger << this->myName<<" has Model ID: " << key << std::endl;
        }

    }
    void ReceiveScheduleTime(const TIME_T currentTime) override{
        logger <<"[Model : (*,t)]" <<"! ! "<<this->myName<<" request (*,"<<currentTime<<") \n";
        CoupledModel::ReceiveScheduleTime(currentTime);
    }
    void ReceiveEvent(Event& event, TIME_T currentTime) override{
        logger <<"[Model : (x,t)]" <<"! ! "<<this->myName<<" received (x,"<<currentTime<<") \n";
        CoupledModel::ReceiveEvent(event, currentTime);
    }
    const TIME_T QueryNextTime() const override{
        logger <<"[Model : TQ]" <<"! ! "<<this->myName<<" query min TA"<<std::endl;
        TIME_T minTime=CoupledModel::QueryNextTime();
        logger <<"[Model : TQ]" <<"! ! "<<this->myName<<" gets min TA : "<<minTime<<std::endl;
        return minTime;
    }
    
};