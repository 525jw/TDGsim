#include "coupled_model.hpp"
#include "event.hpp"

// CoupledModel::CoupledModel(int modelID, Engine* engine) 
// {
//     SetModelID(modelID);
//     SetEngine(engine);
// }
CoupledModel::CoupledModel(Engine* engine) 
{
    SetEngine(engine);
}

bool CoupledModel::AddComponent(Model model) {
    subModels.push_back(model);
    return true;
}

bool CoupledModel::AddCoupling(Model* srcModel, std::string* srcPort, 
                               Model* detModel, std::string* detPort) {
    couplings.emplace_back(srcModel, srcPort, detModel, detPort);
    return true;
}
bool CoupledModel::RemoveCoupling(Model* srcModel, std::string* srcPort,
                                  Model* detModel, std::string* detPort) {
    // Unimplemented
    return false;
}
bool CoupledModel::RemoveCoupling(Model* srcModel, std::string* srcPort) {
    // Unimplemented
    return false;
}

//Handling EIC
void CoupledModel::ReceiveExternalEvent(const Event& externalEvent, TIME_T engineTime){
    TIME_T minTime,newTime;
    minTime=newTime=TIME_INF;
    for (auto& cp : couplings){
        if (cp.getSrcModel() == externalEvent.getSenderModel() && cp.getSrcPort() == externalEvent.getSenderPort()){
            cp.getDetModel()->ReceiveExternalEvent(externalEvent, engineTime); //Broadcasting
            newTime = cp.getDetModel()->QueryNextTime();
            minTime =  newTime < minTime ? newTime : minTime;
        }
    }
    lastTime = engineTime;
    nextTime = minTime;
}
void CoupledModel::ReceiveTimeAdvanceRequest(const TIME_T engineTime){
    for (auto& sm : subModels){
        if(sm.QueryNextTime() == nextTime){
            sm.ReceiveTimeAdvanceRequest(engineTime);
            
        }
    }
}
const TIME_T CoupledModel::QueryNextTime() const{

}