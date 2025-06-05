#include "test_canteam.hpp"

TestCannonTeam::TestCannonTeam(int modelID, Engine* engine, std::string name)
: CoupledModel(modelID, engine)
{
    this->myName=name;
    int idNum = this->GetModelID()*10;

    logger_world    << "["
                << myName
                << "]"
                <<" Created"
                <<std::endl;

    logger_system   << "[" << "TestCannonTeam::"
                    << myName
                    << ":Init]"
                    <<" Created, ID : "<<this->GetModelID()
                    <<std::endl;
    
    TestCannon* firstCannon = new TestCannon(idNum+1, engine,this->myName + "First");
    firstCannon->SetParentModel(this);
    TestCannon* secondCannon = new TestCannon(idNum+2, engine,this->myName + "Second");
    secondCannon->SetParentModel(this);
    TestCannon* thirdCannon = new TestCannon(idNum+3, engine,this->myName + "Third");
    thirdCannon->SetParentModel(this);
    TestCannon* fourthCannon = new TestCannon(idNum+4, engine,this->myName + "Fourth");
    fourthCannon->SetParentModel(this);
    
    this->RegisterModelWithID(firstCannon);
    this->RegisterModelWithID(secondCannon);
    this->RegisterModelWithID(thirdCannon);
    this->RegisterModelWithID(fourthCannon);

    this->AddInputPort("fire_in");
    this->AddOutputPort("fire_out");
    this->AddInputPort("log_ord");

    this->AddCoupling(firstCannon,"fire_out",this,"fire_out",EOC);
    this->AddCoupling(this,"fire_in",firstCannon,"fire_in",EIC);
    this->AddCoupling(this,"log_ord",firstCannon,"log_ord",EIC);

    this->AddCoupling(secondCannon,"fire_out",this,"fire_out",EOC);
    this->AddCoupling(this,"fire_in",secondCannon,"fire_in",EIC);
    this->AddCoupling(this,"log_ord",secondCannon,"log_ord",EIC);

    this->AddCoupling(thirdCannon,"fire_out",this,"fire_out",EOC);
    this->AddCoupling(this,"fire_in",thirdCannon,"fire_in",EIC);
    this->AddCoupling(this,"log_ord",thirdCannon,"log_ord",EIC);

    this->AddCoupling(fourthCannon,"fire_out",this,"fire_out",EOC);
    this->AddCoupling(this,"fire_in",fourthCannon,"fire_in",EIC);
    this->AddCoupling(this,"log_ord",fourthCannon,"log_ord",EIC);

    std::vector<int> keys;
    for (const auto& pair : this->modelsWithID) {
        keys.push_back(pair.first);
    }
    for (const auto& key : keys) {
        logger_system   << "[" << "TestCannonTeam::"
                        << myName
                        << ":Init]" 
                        <<" has Model ID: " << key << std::endl;
    }

    logger_system
        << "[TestCannonTeam::" << myName << ":Init] "
        << "CouplingCount  IC="  << this->CouplingCount(IC)
        << ", EOC="             << this->CouplingCount(EOC)
        << ", EIC="             << this->CouplingCount(EIC)
        << std::endl;


}
void TestCannonTeam::ReceiveScheduleTime(const TIME_T currentTime) {
    logger_system   << "[" << "TestCannonTeam::"
                    << myName
                    << "::ReceiveScheduleTime]"
                    <<" Received (*,"<<currentTime<<")"
                    <<std::endl;
    CoupledModel::ReceiveScheduleTime(currentTime);
}
void TestCannonTeam::ReceiveEvent(Event& event, TIME_T currentTime) {
    logger_system   << "[" << "TestCannonTeam::"
                    << myName
                    << "::ReceiveEvent]"
                    <<" Received (x,"<<currentTime<<")"
                    << std::endl;
    CoupledModel::ReceiveEvent(event, currentTime);
}
const TIME_T TestCannonTeam::QueryNextTime() const {
    logger_system   << "[" << "TestCannonTeam::"
                    << myName
                    << "::QueryNextTime]"
                    <<" query min TA"
                    <<std::endl;
    TIME_T minTime=CoupledModel::QueryNextTime();
    logger_system   << "[" << "TestCannonTeam::"
                    << myName
                    << "::QueryNextTime]"
                    <<" gets min TA : "<<minTime
                    <<std::endl;
    return minTime;
}