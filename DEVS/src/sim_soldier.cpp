#include "sim_soldier.hpp"
#include "sim_decisionmaker.hpp"
#include "sim_maneuver.hpp"

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

    DecisionMaker* dm = new DecisionMaker(idNum+1, engine, this->myName + " DM");
    dm->SetParentModel(this);
    
    Maneuver* maneuver = new Maneuver(idNum+2, engine,this->myName + " Maneuver");
    maneuver->SetParentModel(this);
    
    this->RegisterModelWithID(dm);
    this->RegisterModelWithID(maneuver);

    this->AddCoupling(dm, "m_out", maneuver, "m_in", IC); // DM → Maneuver
    this->AddCoupling(dm, "stop_out", maneuver, "stop_in", IC); // DM → Maneuver
    this->AddCoupling(maneuver, "m_rep", dm, "m_rep", IC); // Maneuver → DM
    this->AddCoupling(maneuver, "m_out", dm, "m_in", IC); // Maneuver → DM

    /*
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
    */

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