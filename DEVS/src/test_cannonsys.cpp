#pragma once
#include "coupled_model.hpp"
#include "test_cannonteam.cpp"
#include "logger.hpp"

class Engine;

class TestCannonSys : public CoupledModel{
public:
    TestCannonSys(int modelID, Engine* engine) 
    : CoupledModel(modelID,engine)
    {
        logger<<"[Model : Init]" <<"! "<<"cannon system created id:"<<this->GetModelID()<<"\n";
        TestCannonTeam* blueteam = new TestCannonTeam(1, engine,"blue");
        TestCannonTeam* redteam = new TestCannonTeam(2, engine,"red");

        this->RegisterModelWithID(blueteam);
        this->RegisterModelWithID(redteam);

        this->AddCoupling(blueteam, "fire_out", redteam, "fire_in", IC);
        this->AddCoupling(redteam, "fire_out", blueteam, "fire_in", IC);

        std::vector<int> keys;
        for (const auto& pair : this->modelsWithID) {
            keys.push_back(pair.first);
        }
        for (const auto& key : keys) {
            logger <<"[Model : Init] "<<" sys has Model ID: " << key << std::endl;
        }
    }
    void ReceiveScheduleTime(const TIME_T currentTime) override{
        logger<<"[Model : (*,t)]" <<"! "<<"cannon system request (*,"<<currentTime<<") \n";
        CoupledModel::ReceiveScheduleTime(currentTime);
    }
    const TIME_T QueryNextTime() const override{
        logger<<"[Model : TQ]" <<"! "<<"cannon system query min TA"<<std::endl;
        TIME_T minTime=CoupledModel::QueryNextTime();
        logger<<"[Model : TQ]" <<"! "<<"cannon system gets min TA : "<<minTime<<std::endl;
        return minTime;
    }
    void ReceiveEvent(Event& externalEvent, TIME_T currentTime){
        logger<<"[Model : (x,t)]" <<"! "<<"cannon sys recevied event "<<externalEvent.getSenderModelID()<<externalEvent.getSenderPort()<<std::endl;
        CoupledModel::ReceiveEvent(externalEvent, currentTime);
    }
};