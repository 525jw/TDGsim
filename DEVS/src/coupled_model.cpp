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
        Model* srcModel, const std::string& srcPort, 
        Model* detModel, const std::string& detPort,
        CouplingType type
) {
    if (type != IC && type != EOC && type != EIC) {
        std::cerr << "[AddCoupling] CouplingType must be IC/EOC/EIC.\n";
        return false;
    }
    if (type == EIC && srcModel != this) {
        std::cerr << "[AddCoupling] For EIC, the source model must be the coupled model itself.\n";
        return false;
    }
    couplings[type].emplace_back(std::make_unique<Coupling>(srcModel, srcPort, detModel, detPort));
    return true;
}
// NOTE : 아직 기능 검증되지 않음, 사용을 원한다면 sim 결과의 출력 방식을 변경 필요 (event 처리 방식 -> engine에서 직접 출력요청)
bool CoupledModel::RemoveCoupling(Model* srcModel, std::string* srcPort,
                                  Model* detModel, std::string* detPort) {
    bool removed = false;
    for (auto& [type, vec] : couplings) {
        const auto oldSize = vec.size();

        vec.erase(std::remove_if(vec.begin(), vec.end(),
            [&](const std::unique_ptr<Coupling>& cp) {
                if (srcModel && cp->getSrcModel() != srcModel)       return false;
                if (srcPort  && cp->getSrcPort()  != *srcPort)       return false;
                if (detModel && cp->getDetModel() != detModel)       return false;
                if (detPort  && cp->getDetPort()  != *detPort)       return false;
                return true;
            }),
            vec.end());

        if (vec.size() != oldSize)
            removed = true;
    }
    return removed;
}
bool CoupledModel::RemoveCoupling(Model* srcModel, std::string* srcPort) {
    return RemoveCoupling(srcModel, srcPort, nullptr, nullptr);
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

    logger_system   << "[CoupledModel::RouteEIC] Starts"
                    << " SrcModel : " << event.getSenderModelID()
                    << " SrcPort : " << event.getSenderPort()
                    << std::endl;

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

    logger_system   << "[CoupledModel::RouteEOC] Starts"
                    << " SrcModel : " << event.getSenderModelID()
                    << " SrcPort : " << event.getSenderPort()
                    << std::endl;

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
            
            if(this->GetParentModel() != nullptr)
                this->GetParentModel()->ReceiveEvent(ev, currentTime);
        }
    }
}
void CoupledModel::RouteIC(Event& event, TIME_T currentTime){  // Handling IC

    logger_system   << "[CoupledModel::RouteIC] Starts"
                    << " SrcModel : " << event.getSenderModelID()
                    << " SrcPort : " << event.getSenderPort()
                    << std::endl;
    
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
            }
            cp->getDetModel()->ReceiveEvent(ev, currentTime);
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