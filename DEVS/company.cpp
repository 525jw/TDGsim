#include "company.hpp"

Company::Company(int modelID, Engine* engine, std::string name)
: CoupledModel(modelID, engine)
{
    this->myName=name;
    int idNum = this->GetModelID()*10;

    logger_world    << "["
                << myName
                << "]"
                <<" Created"
                <<std::endl;

    logger_system   << "[" << "Company::"
                    << myName
                    << ":Init]"
                    <<" Created, ID : "<<this->GetModelID()
                    <<std::endl;
    
    Generator* generator = new Generator(1, engine, "Generator");
    generator->SetParentModel(this);
    DataCollector* data_collector = new DataCollector(2, engine, "DataCollector");
    data_collector->SetParentModel(this);
    
    this->RegisterModelWithID(generator);
    this->RegisterModelWithID(data_collector);

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
        logger_system   << "[" << "Company::"
                        << myName
                        << ":Init]" 
                        <<" has Model ID: " << key << std::endl;
    }

    logger_system
        << "[Company::" << myName << ":Init] "
        << "CouplingCount  IC="  << this->CouplingCount(IC)
        << ", EOC="             << this->CouplingCount(EOC)
        << ", EIC="             << this->CouplingCount(EIC)
        << std::endl;

}
void Company::ReceiveScheduleTime(const TIME_T currentTime) {
    logger_system   << "[" << "Company::"
                    << myName
                    << "::ReceiveScheduleTime]"
                    <<" Received (*,"<<currentTime<<")"
                    <<std::endl;
    CoupledModel::ReceiveScheduleTime(currentTime);
}
void Company::ReceiveEvent(Event& event, TIME_T currentTime) {
    logger_system   << "[" << "Company::"
                    << myName
                    << "::ReceiveEvent]"
                    <<" Received (x,"<<currentTime<<")"
                    << std::endl;
    CoupledModel::ReceiveEvent(event, currentTime);
}
const TIME_T Company::QueryNextTime() const {
    logger_system   << "[" << "Company::"
                    << myName
                    << "::QueryNextTime]"
                    <<" query min TA"
                    <<std::endl;
    TIME_T minTime=CoupledModel::QueryNextTime();
    logger_system   << "[" << "Company::"
                    << myName
                    << "::QueryNextTime]"
                    <<" gets min TA : "<<minTime
                    <<std::endl;
    return minTime;
}