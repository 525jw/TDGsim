#include "coupled_model.hpp"
#include "event.hpp"
#include "engine.hpp"

CoupledModel::CoupledModel(int modelID, Engine* engine)
    : Model(modelID, engine)
{
    this->engine->RegisterModelWithID(this);
}

bool CoupledModel::AddCoupling(
        Model* srcModel, std::string srcPort, 
        Model* detModel, std::string detPort,
        CouplingType type
) {
    CouplingType tp;

    if (type == IC || type == EOC || type == EIC) {
        if(type == EIC && srcModel->GetModelID() != this->GetModelID())
            std::cerr << "[AddCoupling] For EIC, the source model must be the coupled model itself.\n";
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
void CoupledModel::ReceiveEvent(Event& event, TIME_T currentTime){ // when receive (x,t)
    if(this->lastTime <= currentTime && currentTime <= this->nextTime){
        if (std::find(this->GetInputPorts().begin(), this->GetInputPorts().end(), event.getSenderPort()) != this->GetInputPorts().end()){
            this->RouteEIC(event, currentTime);
        }else{
            this->RouteIC(event,currentTime);

        }
    }else{
        // ERROR : event를 free 해야함
    }
}
void CoupledModel::RouteEIC(Event& event, TIME_T currentTime){
    std::vector<int> connected;
    for (auto& cp : this->couplings[EIC]) { // Handling EIC
        if (cp->getSrcModel()->GetModelID() == event.getSenderModelID() && cp->getSrcPort() == event.getSenderPort()){
            connected.push_back(cp->getDetModel()->GetModelID());
            // if (cp->getSrcModel()->IsCoupled())
            //     this->Translate(event, cp->getDetModel()->GetModelID(), cp->getDetPort());
            // cp->getDetModel()->ReceiveEvent(event, currentTime);
        }
    }
    for (auto cid : connected) {
        const auto& mid = modelsWithID[cid];
        Event* routed = new Event(event);
        routed->SetSenderModelID(this->GetModelID());
        routed->SetSenderPort(cp->GetInPort());
        cp->ReceiveEvent(*routed, currentTime);
        pendingFree_.push_back(routed);
    }
    delete &event;
    // update time
    this->lastTime = currentTime;
    for (auto& cid : connected)
        this->nextTime = std::min(this->nextTime, modelsWithID[cid]->GetNextTime());
}
void CoupledModel::RouteEOC(Event& event, TIME_T currentTime){

}
void CoupledModel::RouteIC(Event& event, TIME_T currentTime){
    for (auto& cp : this->couplings[IC]) { // Handling IC
        if (cp->getSrcModel()->GetModelID() == event.getSenderModelID() && cp->getSrcPort() == event.getSenderPort()){
            if (cp->getDetModel()->IsCoupled())
                this->Translate(event, cp->getDetModel()->GetModelID(), cp->getDetPort());
            cp->getDetModel()->ReceiveEvent(event, currentTime);
        }
    }
}

void CoupledModel::ReceiveScheduleTime(const TIME_T currentTime){ // when receive (*,t)
    if(currentTime == this->nextTime){
        for (auto& mid : modelsWithID){
            if(mid.second->GetNextTime() == this->nextTime){
                mid.second->ReceiveScheduleTime(currentTime);
            }
        }
        this->lastTime = currentTime;
    }else{
        // ERROR
    }
}
// - Upadate the Sender Info(ID, Port) of the Event
void CoupledModel::Translate(Event& event, int srcModelID, std::string& srcPort){
    event.SetSenderModelID(srcModelID);
    event.SetSenderPort(srcPort);
}
// void CoupledModel::SendOutputEvent(Event& outputEvent){
//     for (auto& cp : this->couplings[EOC]) {
//         if (cp->getSrcModel()->GetModelID() == outputEvent.getSenderModelID() && cp->getSrcPort() == outputEvent.getSenderPort()){
//             this->Translate(outputEvent, cp->getDetModel()->GetModelID(), cp->getDetPort());
//             cp->getDetModel()->SendOutputEvent(&outputEvent);
//             return;
//         }
//     }
//     for (auto& cp : this->couplings[IC]) {
//         if (cp->getSrcModel()->GetModelID() == outputEvent.getSenderModelID() && cp->getSrcPort() == outputEvent.getSenderPort()){
//             this->Translate(outputEvent, cp->getDetModel()->GetModelID(), cp->getDetPort());
//             cp->getDetModel()->SendOutputEvent(&outputEvent);
//             return;
//         }
//     }
//     this->engine->AddEvent(&outputEvent);
// }

// const TIME_T CoupledModel::QueryNextTime() const{
//     TIME_T minTime=TIME_INF;
//     for (auto& mid : modelsWithID)
//         minTime = std::min(minTime, mid.second->QueryNextTime());
//     return minTime;
// }

bool CoupledModel::RegisterModelWithID(Model* model) {
    // NOTE : engine의 RegisterMoelWithID와 코드 중복 (modelWithID 구조 동일)
    int id = model->GetModelID();
    modelsWithID[id] = model;
    return true;
}