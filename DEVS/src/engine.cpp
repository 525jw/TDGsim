#include "engine.hpp"

Engine::Engine() 
    : currentTime(0.0) {}

void Engine::BuildDEVS(Model* rootModel){
    this->rootModel = rootModel;
}

void Engine::Run(){

    std::vector<int> keys;
    for (const auto& pair : modelsWithID) {
        keys.push_back(pair.first);
    }
    for (const auto& key : keys) {
        logger_system << "[Engine::Run]"<< " Prepared Model ID : " << key << std::endl;
    }

    while(this->currentTime<10.0){ //// TODO : needs while loop unitl the end time
        std::cout       << "[Engine::Run]"<< " >>> Engine running "
                        << "CurTime : " << this->currentTime<<", Event queue size : "<<this->eventQueue.size()
                        << " <<< "
                        << std::endl;

        logger_system   << "[Engine::Run]"<< " >>> Engine running "
                        << "CurTime : " << this->currentTime<<", Event queue size : "<<this->eventQueue.size()
                        << " <<< "
                        << std::endl;

        logger_world    << "[Engine]"
                        << " Current Time is " << this->currentTime
                        << std::endl;

        if(this->eventQueue.empty()){
            logger_system   << "[Engine::Run] Event queue is empty, "
                            << "Query nextTime"<<std::endl;
            
            TIME_T minTA = this->rootModel->QueryNextTime();
            if(minTA > TIME_INF)
                return;
            this->currentTime = minTA;

            logger_system   << "[Engine::Run] Received minTA = "<<minTA
                            << "Request (*,"<<this->currentTime<<")"<<std::endl;
            this->rootModel->ReceiveScheduleTime(this->currentTime);
        }else{
            logger_system << "[Engine::Run] Starts broadcasting events"<<std::endl;
            
            Event* currentEvent = nullptr;
            Model* curModel = nullptr;
            while(!this->eventQueue.empty()){
                currentEvent = this->eventQueue.front(); this->eventQueue.pop();

                if (currentEvent == nullptr) {
                    logger_system << "[ERROR] CurrentEvent is nullptr!"<<std::endl;
                    continue;
                }

                curModel = this->modelsWithID[currentEvent->getSenderModelID()];
                if (curModel == nullptr) {
                    logger_system << "[ERROR] CurrentEvent->getSenderModel() is nullptr!"<<std::endl;
                    continue;
                }

                logger_system   << "[Engine::Run] Request (x,"<<this->currentTime<<")"<<std::endl;

                curModel->GetParentModel()->ReceiveEvent(*currentEvent, this->currentTime);
            }
        }
    }
}

void Engine::AddEvent(Event* event){
    logger_system   <<"[Engine::AddEvent]"<<" Event in, "
                    <<"SenderModel : "<<event->getSenderModelID()
                    <<"SenderPort : "<<event->getSenderPort()
                    <<std::endl;

    this->eventQueue.push(event);
}
bool Engine::RegisterModelWithID(Model* model) {
    // DISCUSS : engine에서 등록할때 ID를 배정해줄지, ID 받을지 논의 후 결정, 현재는 id를 받음
    int id = model->GetModelID();
    modelsWithID[id] = model;
    return true;
}
