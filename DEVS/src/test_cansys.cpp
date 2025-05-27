#include "test_cansys.hpp"

TestCannonSys::TestCannonSys(int modelID, Engine* engine) 
: CoupledModel(modelID,engine)
{

    logger_world    << "["
                    << myName
                    << "]"
                    <<" Created"
                    <<std::endl;

    logger_system   << "[" << "TestCannonSys::"
                    << myName
                    << ":Init]"
                    <<" Created, ID : "<<this->GetModelID()
                    <<std::endl;

    this->myName = "Sys";
    TestCannonTeam* blueteam = new TestCannonTeam(1, engine,"Blue");
    blueteam->SetParentModel(this);
    TestCannonTeam* redteam = new TestCannonTeam(2, engine,"Red");
    redteam->SetParentModel(this);

    this->RegisterModelWithID(blueteam);
    this->RegisterModelWithID(redteam);

    this->AddInputPort("log_ord");

    this->AddCoupling(this, "log_ord", blueteam, "log_ord", EIC);
    this->AddCoupling(this, "log_ord", redteam, "log_ord", EIC);

    this->AddCoupling(blueteam, "fire_out", redteam, "fire_in", IC);
    this->AddCoupling(redteam, "fire_out", blueteam, "fire_in", IC);
    
    std::vector<int> keys;
    for (const auto& pair : this->modelsWithID) {
        keys.push_back(pair.first);
    }
    for (const auto& key : keys) {
        logger_system   << "[" << "TestCannonSys::"
                        << myName
                        << ":Init]" 
                        <<" has Model ID: " << key << std::endl;
    }

    logger_system
        << "[TestCannonSys::" << myName << ":Init] "
        << "CouplingCount  IC="  << this->CouplingCount(IC)
        << ", EOC="             << this->CouplingCount(EOC)
        << ", EIC="             << this->CouplingCount(EIC)
        << std::endl;

}
void TestCannonSys::ReceiveScheduleTime(const TIME_T currentTime) {
    logger_system   << "[" << "TestCannonSys::"
                    << myName
                    << "::ReceiveScheduleTime]"
                    <<" Received (*,"<<currentTime<<")"
                    <<std::endl;
    CoupledModel::ReceiveScheduleTime(currentTime);
}
const TIME_T TestCannonSys::QueryNextTime() const {
    logger_system   << "[" << "TestCannonSys::"
                    << myName
                    << "::QueryNextTime]"
                    <<" query min TA"
                    <<std::endl;
    TIME_T minTime=CoupledModel::QueryNextTime();
    logger_system   << "[" << "TestCannonSys::"
                    << myName
                    << "::QueryNextTime]"
                    <<" gets min TA : "<<minTime
                    <<std::endl;
    return minTime;
}
void TestCannonSys::ReceiveEvent(Event& externalEvent, TIME_T currentTime){
    logger_system   << "[" << "TestCannonSys::"
                    << myName
                    << "::ReceiveEvent]"
                    <<" Received (x,"<<currentTime<<")"
                    << std::endl;
    CoupledModel::ReceiveEvent(externalEvent, currentTime);
}