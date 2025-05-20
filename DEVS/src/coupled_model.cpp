#include "coupled_model.hpp"
#include "event.hpp"
#include "engine.hpp"

CoupledModel::CoupledModel(int modelID, Engine* engine)
{
    SetModelID(modelID);
    SetEngine(engine);
    this->engine->RegisterModelWithID(this);
}

bool CoupledModel::AddCoupling(
        Model* srcModel, std::string srcPort, 
        Model* detModel, std::string detPort,
        CouplingType type
) {
    CouplingType tp;

    if (type == IC || type == EOC || type == EIC) {
        tp = type;
    } else { 
        // TODO: automatic CouplingType inference
        std::cerr << "[AddCoupling] Please explicitly provide one of the following types: EIC, EOC, IC.\n";
    }
    couplings[tp].emplace_back(new Coupling(srcModel, srcPort, detModel, detPort));
    return true;
}
bool CoupledModel::RemoveCoupling(Model* srcModel, std::string* srcPort,
                                  Model* detModel, std::string* detPort) {
    // TODO : RemoveCoupling Unimplemented
    return false;
}
bool CoupledModel::RemoveCoupling(Model* srcModel, std::string* srcPort) {
    // TODO : RemoveCoupling Unimplemented
    return false;
}

//Handling EIC
void CoupledModel::ReceiveExternalEvent(const Event& externalEvent, TIME_T engineTime){
    if(this->lastTime <= engineTime && engineTime <= this->nextTime){
        TIME_T minTime,newTime;
        minTime=newTime=TIME_INF;
        for (auto& cp : couplings[EIC]) {
            if (cp->getSrcModel()->GetModelID() == externalEvent.getSenderModel()->GetModelID() && cp->getSrcPort() == externalEvent.getSenderPort()){
                cp->getDetModel()->ReceiveExternalEvent(externalEvent, engineTime); //Broadcasting
                newTime = cp->getDetModel()->QueryNextTime();
                minTime =  newTime < minTime ? newTime : minTime;
            }
        }
        lastTime = engineTime;
        nextTime = minTime; // TODO : QueryNextTime과 중복코드 여지 존재
    }else{
        // ERROR
    }
}
void CoupledModel::ReceiveTimeAdvanceRequest(const TIME_T engineTime){
    if(engineTime >= this->nextTime){
        for (auto& mid : modelsWithID){
            mid.second->ReceiveTimeAdvanceRequest(engineTime);
        }
        lastTime = engineTime;
        nextTime = QueryNextTime();
    }
}
const TIME_T CoupledModel::QueryNextTime() const{
    TIME_T minTime,newTime;
    minTime=newTime=TIME_INF;
    for (auto& mid : modelsWithID){
        newTime=mid.second->QueryNextTime();
        minTime = newTime < minTime ? newTime : minTime;
    }
    return minTime;
}

const int CoupledModel::GetComponentSize() const{
    return this->modelsWithID.size();
}

bool CoupledModel::RegisterModelWithID(Model* model) {
    // TODO : engine에서 등록할때 ID를 배정해줄지, ID 받을지 논의 후 결정, 현재는 id를 받음, engine과 코드 중복
    int id = model->GetModelID();
    modelsWithID[id] = model;
    return true;
}