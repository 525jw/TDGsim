#include "coupled_model.hpp"
#include "event.hpp"
#include "engine.hpp"
#include "logger.hpp"

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
            // std::cerr << "[AddCoupling] For EIC, the source model must be the coupled model itself.\n";
        tp = type;
    } else { 
        // TODO: automatic CouplingType inference and changes error message
        // std::cerr << "[AddCoupling] Please explicitly provide one of the following types: EIC, EOC, IC.\n";
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
            this->RouteEOC(event,currentTime);
        }

        //update time, NOTE : potential bottleneck in QueryNextTime
        this->lastTime = currentTime;
        this->nextTime = QueryNextTime();
    }else{
        // ERROR
        // TODO : event를 free 해야함
    }
}
void CoupledModel::RouteEIC(Event& event, TIME_T currentTime){  // Handling EIC

    logger_system << "[CoupledModel::RouteEIC] Starts"<< std::endl;

    for (auto& cp : this->couplings[EIC]) {
        if (cp->getSrcModel()->GetModelID() == event.getSenderModelID() && cp->getSrcPort() == event.getSenderPort()){

            logger_system   << "[CoupledModel::RouteEIC] Found Matching coupling: "
                            << "from (" << cp->getSrcModel()->GetModelID() << ", " << cp->getSrcPort() << ") → "
                            << "to (" << cp->getDetModel()->GetModelID() << ", " << cp->getDetPort() << ")"
                            << std::endl;

            Event ev = event;
            if (cp->getSrcModel()->IsCoupled()){
                ev = this->Translate(event, cp->getDetModel()->GetModelID(), cp->getDetPort());
            
                logger_system   << "[CoupledModel::RouteEIC] Translated : "
                                << "from (" << cp->getSrcModel()->GetModelID() << ", " << cp->getSrcPort() << ") → "
                                << "to (" << cp->getDetModel()->GetModelID() << ", " << cp->getDetPort() << ")"
                                << std::endl;
            
            }
            cp->getDetModel()->ReceiveEvent(ev, currentTime);
        }
    }
}
void CoupledModel::RouteEOC(Event& event, TIME_T currentTime){  // Handling EOC

    logger_system << "[CoupledModel::RouteEOC] Starts"<< std::endl;

    for (auto& cp : this->couplings[EOC]) {
        if (cp->getSrcModel()->GetModelID() == event.getSenderModelID() && cp->getSrcPort() == event.getSenderPort()){

            logger_system   << "[CoupledModel::RouteEOC] Found Matching coupling: "
                            << "from (" << cp->getSrcModel()->GetModelID() << ", " << cp->getSrcPort() << ") → "
                            << "to (" << cp->getDetModel()->GetModelID() << ", " << cp->getDetPort() << ")"
                            << std::endl;

            Event ev = this->Translate(event, cp->getDetModel()->GetModelID(), cp->getDetPort());

            logger_system   << "[CoupledModel::RouteEOC] Translated : "
                            << "from (" << cp->getSrcModel()->GetModelID() << ", " << cp->getSrcPort() << ") → "
                            << "to (" << cp->getDetModel()->GetModelID() << ", " << cp->getDetPort() << ")"
                            << std::endl;

            cp->getDetModel()->ReceiveEvent(ev, currentTime);
        }
    }
}
void CoupledModel::RouteIC(Event& event, TIME_T currentTime){  // Handling IC

    logger_system << "[CoupledModel::RouteIC] Starts"<< std::endl;
    
    for (auto& cp : this->couplings[IC]) {
        if (cp->getSrcModel()->GetModelID() == event.getSenderModelID() && cp->getSrcPort() == event.getSenderPort()){

            logger_system   << "[CoupledModel::RouteIC] Found Matching coupling: "
                            << "from (" << cp->getSrcModel()->GetModelID() << ", " << cp->getSrcPort() << ") → "
                            << "to (" << cp->getDetModel()->GetModelID() << ", " << cp->getDetPort() << ")"
                            << std::endl;

            Event ev = event;
            if (cp->getDetModel()->IsCoupled()){
                ev = this->Translate(event, cp->getDetModel()->GetModelID(), cp->getDetPort());

                logger_system   << "[CoupledModel::RouteIC] Translated : "
                                << "from (" << cp->getSrcModel()->GetModelID() << ", " << cp->getSrcPort() << ") → "
                                << "to (" << cp->getDetModel()->GetModelID() << ", " << cp->getDetPort() << ")"
                                << std::endl;

                this->engine->AddEvent(&ev);
            }else{
                cp->getDetModel()->ReceiveEvent(ev, currentTime);
            }
        }
    }
}
// Return a copy of the event with updated sender ID and port
Event CoupledModel::Translate(const Event& in, int srcModelID, const std::string& srcPort){

    logger_system << "[CoupledModel::Translate]"<< std::endl;

    Event out = in;
    out.setSenderModelID(srcModelID);
    out.setSenderPort(srcPort);
    return out;
}
void CoupledModel::ReceiveScheduleTime(const TIME_T currentTime){ // when receive (*,t)
    if(currentTime == this->nextTime){
        for (auto& mid : modelsWithID){
            if(mid.second->GetNextTime() == this->nextTime){
                mid.second->ReceiveScheduleTime(currentTime);
            }
        }
        //update time, NOTE : potential bottleneck in QueryNextTime
        this->lastTime = currentTime;
        this->nextTime = QueryNextTime();
    }else{
        // ERROR
    }
}

const TIME_T CoupledModel::QueryNextTime() const{
    TIME_T minTime=TIME_INF;
    for (auto& mid : modelsWithID)
        minTime = std::min(minTime, mid.second->QueryNextTime());
    return minTime;
}

bool CoupledModel::RegisterModelWithID(Model* model) {
    // NOTE : engine의 RegisterMoelWithID와 코드 중복 (modelWithID 구조 동일)
    int id = model->GetModelID();
    modelsWithID[id] = model;
    return true;
}