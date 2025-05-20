#include "coupled_model.hpp"
#include "event.hpp"
#include "engine.hpp"

CoupledModel::CoupledModel(int modelID, Engine* engine)
    : Model(modelID, engine)
{
    this->engine->RegisterModelWithID(this);
    // TODO : ID 중 하나를 atomic coupled의 flag bit로 사용하는 방법 고려
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
void CoupledModel::Translate(Event& event, int srcModelID, std::string& srcPort){
    event.SetSenderModelID(srcModelID);
    event.SetSenderPort(srcPort);
}
//Handling EIC
void CoupledModel::ReceiveExternalEvent(Event& externalEvent, TIME_T engineTime){
    if(this->lastTime <= engineTime && engineTime <= this->nextTime){
        for (auto& cp : couplings[EIC]) {
            if (cp->getSrcModel()->GetModelID() == externalEvent.getSenderModelID() && cp->getSrcPort() == externalEvent.getSenderPort()){
                /*
                    Event propagation via nested EICs — translation required at each coupled boundary:
    
                    A:red_soldier     , fire_out  -> C:blue_company , fire_in (EIC)
                    C:blue_company    , fire_in   -> C:blue_squad   , fire_in (EIC)
                    C:blue_squad      , fire_in   -> A:blue_soldier , fire_in (EIC)

                    => delegate translation to its scope
                */
                this->Translate(externalEvent, cp->getDetModel(), cp->getDetPort());
                cp->getDetModel()->ReceiveExternalEvent(externalEvent, engineTime); //Broadcasting
            }
        }
        // update time
    }else{
        // ERROR
    }
}
void CoupledModel::ReceiveTimeAdvanceRequest(const TIME_T engineTime){
    if(engineTime == this->nextTime){ // if t=tN then
        for (auto& mid : modelsWithID){ // find component(s) with tN
            if(this->nextTime == mid.second->nextTime){
                
            }
        }
        // for (auto& mid : modelsWithID){
        //     mid.second->ReceiveTimeAdvanceRequest(engineTime);
        // }
        // lastTime = engineTime;
        // nextTime = QueryNextTime();
    }else{
        // ERROR
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