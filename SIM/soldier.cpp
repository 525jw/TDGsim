#include "soldier.hpp"

Soldier::Soldier(int modelID, Engine* engine, std::string name)
: CoupledModel(modelID, engine)
{
    this->myName=name;
    int idNum = this->GetModelID()*10;

    logger_world    << "["
                << myName
                << "]"
                <<" Created"
                <<std::endl;

    logger_system   << "[" << "Soldier::"
                    << myName
                    << ":Init]"
                    <<" Created, ID : "<<this->GetModelID()
                    <<std::endl;
    
    Maneuver* maneuver = new Maneuver(123456789, engine, "Maneuver");
    maneuver->SetParentModel(this);

    this->RegisterModelWithID(maneuver);

    this->AddInputPort("result");
    this->AddOutputPort("start");

    this->AddCoupling(generator,"start",this,"start",EOC);
    this->AddCoupling(this,"result",data_collector,"result",EIC);
    this->AddCoupling(data_collector,"restart",generator,"restart",IC);

    std::vector<int> keys;
    for (const auto& pair : this->modelsWithID) {
        keys.push_back(pair.first);
    }
    for (const auto& key : keys) {
        logger_system   << "[" << "Soldier::"
                        << myName
                        << ":Init]" 
                        <<" has Model ID: " << key << std::endl;
    }

    logger_system
        << "[Soldier::" << myName << ":Init] "
        << "CouplingCount  IC="  << this->CouplingCount(IC)
        << ", EOC="             << this->CouplingCount(EOC)
        << ", EIC="             << this->CouplingCount(EIC)
        << std::endl;

}
void Soldier::ReceiveScheduleTime(const TIME_T currentTime) {
    logger_system   << "[" << "Soldier::"
                    << myName
                    << "::ReceiveScheduleTime]"
                    <<" Received (*,"<<currentTime<<")"
                    <<std::endl;
    CoupledModel::ReceiveScheduleTime(currentTime);
}
void Soldier::ReceiveEvent(Event& event, TIME_T currentTime) {
    logger_system   << "[" << "Soldier::"
                    << myName
                    << "::ReceiveEvent]"
                    <<" Received (x,"<<currentTime<<")"
                    << std::endl;
    CoupledModel::ReceiveEvent(event, currentTime);
}
const TIME_T Soldier::QueryNextTime() const {
    logger_system   << "[" << "Soldier::"
                    << myName
                    << "::QueryNextTime]"
                    <<" query min TA"
                    <<std::endl;
    TIME_T minTime=CoupledModel::QueryNextTime();
    logger_system   << "[" << "Soldier::"
                    << myName
                    << "::QueryNextTime]"
                    <<" gets min TA : "<<minTime
                    <<std::endl;
    return minTime;
}