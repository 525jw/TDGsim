#pragma once
#include "coupled_model.hpp"
#include "test_cannon.cpp"
#include "logger.hpp"
#include <string>

class TestCannonTeam : public CoupledModel{
private:
    std::string myName;
public:
    TestCannonTeam(int modelID, Engine* engine,std::string name)
    : CoupledModel(modelID,engine)
    {
        this->myName=name;
        logger <<"! ! "<<myName<<" created id:"<<this->GetModelID()<<"\n";
        int idNum = this->GetModelID()*10;
        
        TestCannon* firstCannon = new TestCannon(idNum+1, engine,this->myName + "first");
        TestCannon* secondCannon = new TestCannon(idNum+2, engine,this->myName + "second");
        TestCannon* thirdCannon = new TestCannon(idNum+3, engine,this->myName + "third");
        
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
    void ReceiveTimeAdvanceRequest(const TIME_T engineTime) override{
        logger <<"! ! "<<this->myName<<" request (*,"<<engineTime<<") \n";
        CoupledModel::ReceiveTimeAdvanceRequest(engineTime);
    }
    const TIME_T QueryNextTime() const override{
        logger <<"! ! "<<this->myName<<" query min TA"<<std::endl;
        TIME_T minTime=CoupledModel::QueryNextTime();
        logger <<"! ! "<<this->myName<<" gets min TA : "<<minTime<<std::endl;
        return minTime;
    }
};