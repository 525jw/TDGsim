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

bool CoupledModel::AddComponent(Model* model) {
    subModels.push_back(model);
    return true;
}

bool CoupledModel::AddCoupling(
        Model* srcModel, std::string* srcPort, 
        Model* detModel, std::string* detPort,
        CouplingType* type = nullptr
) {
    CouplingType tp;

    if (type != nullptr) {
        tp = *type;
    } else { 
        // TODO: automatic CouplingType inference
        std::cerr << "[AddCoupling] Please explicitly provide one of the following types: EIC, EOC, IC.\n";
    }
    couplings[tp].emplace_back(srcModel, srcPort, detModel, detPort);
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
    for (auto& cp : couplings[CouplingType :: EIC]){
        if (cp->getSrcModel() == externalEvent.getSenderModel() && cp->getSrcPort() == externalEvent.getSenderPort()){
            cp->getDetModel()->ReceiveExternalEvent(externalEvent, engineTime); //Broadcasting
            newTime = cp->getDetModel()->QueryNextTime();
            minTime =  newTime < minTime ? newTime : minTime;
        }
    }
    lastTime = engineTime;
    nextTime = minTime; // TODO : QueryNextTime과 중복코드 여지 존재
}
void CoupledModel::ReceiveTimeAdvanceRequest(const TIME_T engineTime){
    for (auto& sm : subModels){
        ReceiveTimeAdvanceRequest(engineTime);
        lastTime = engineTime;
        nextTime = QueryNextTime();
    }
}
const TIME_T CoupledModel::QueryNextTime() const{
    TIME_T minTime,newTime;
    minTime=newTime=TIME_INF;
    for (auto& sm : subModels){
        newTime=sm->QueryNextTime();
        minTime = newTime < minTime ? newTime : minTime;
    }
    return minTime;
}