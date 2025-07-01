#include "engine.hpp"

Engine::Engine() 
    : currentTime(0.0) {}

void Engine::BuildDEVS(Model* rootModel){
    this->rootModel = rootModel;
}

void Engine::Run(){

    // DEBUG ONLY -----------
    int countFire = 0;
    // ----------------------

    std::cout       << "[Engine::Run]"<< " >>> Engine starts "
                    << "CurTime : " << this->currentTime<<", Event queue size : "<<this->eventQueue.size()
                    << " <<< "
                    << std::endl;

    std::vector<int> keys;
    for (const auto& pair : modelsWithID) {
        keys.push_back(pair.first);
    }
    for (const auto& key : keys) {
        logger_system << "[Engine::Run]"<< " Prepared Model ID : " << key << std::endl;
    }

    while(this->currentTime<30.0){ //// TODO : needs while loop unitl the end time
        std::cout       << "[Engine::Run]"<< " Engine running " 
                        << "CurTime : " << this->currentTime<<", Event queue size : "<<this->eventQueue.size()
                        << std::endl;

        logger_system   << "[Engine::Run]"<< " Engine running \n"
                        << "------------------------------------"
                        << "CurTime : " << this->currentTime<<", Event queue size : "<<this->eventQueue.size()
                        << "------------------------------------"
                        << std::endl;

        if(lastTime > currentTime){
            // ERROR
            exit(1);
        }

        if(this->eventQueue.empty()){
            logger_system   << "[Engine::Run] Event queue is empty, "
                            << "Query nextTime"<<std::endl;

            logger_world    << "Fire Turn " << this->currentTime
                            << std::endl;
            
            TIME_T minTA = this->rootModel->QueryNextTime();
            if(minTA > TIME_INF)
                return;
            this->lastTime = this->currentTime;
            this->currentTime = minTA;

            logger_system   << "[Engine::Run] Received minTA = "<<minTA
                            << " Request (*,"<<this->currentTime<<")"<<std::endl;
            this->rootModel->ReceiveScheduleTime(this->currentTime);
        }else{
            logger_system   << "[Engine::Run] Starts broadcasting events "<<std::endl;

            logger_world    << "Damage Turn " << this->currentTime
                            << std::endl;
            
            Event* curEvent = nullptr;
            Model* curModel = nullptr;
            while(!this->eventQueue.empty()){
                curEvent = this->eventQueue.front(); this->eventQueue.pop();

                if (curEvent == nullptr) {
                    logger_system << "[ERROR] CurrentEvent is nullptr!"<<std::endl;
                    continue;
                }

                curModel = this->modelsWithID[curEvent->getSenderModelID()];
                if (curModel == nullptr) {
                    logger_system << "[ERROR] CurrentEvent->getSenderModel() is nullptr!"<<std::endl;
                    continue;
                }

                logger_system   << "[Engine::Run] Request (x,"<<this->currentTime<<")"
                                <<" SenderModel : " <<curEvent->getSenderModelID()
                                <<" SenderPort : "  <<curEvent->getSenderPort()
                                <<std::endl;
                                
                countFire ++; // DEBUG ONLY
                curModel->GetParentModel()->ReceiveEvent(*curEvent, this->currentTime);
            }
        }
    }

    // Print Result
    this->RequestLogAllStatus();

    logger_world << "Total Fire Turns : "<< countFire << std::endl; // DEBUG ONLY

    std::cout       << "[Engine::Run]"<< " >>> Engine Ends "
                    << "CurTime : " << this->currentTime<<", Event queue size : "<<this->eventQueue.size()
                    << " <<< "
                    << std::endl;
}

void Engine::AddEvent(Event* event){
    logger_system   <<"[Engine::AddEvent]"<<" Event in,"
                    <<" SenderModel : "<<event->getSenderModelID()
                    <<" SenderPort : "<<event->getSenderPort()
                    <<std::endl;

    this->eventQueue.push(event);
}
bool Engine::RegisterModelWithID(Model* model) {
    // DISCUSS : engine에서 등록할때 ID를 배정해줄지, ID 받을지 논의 후 결정, 현재는 id를 받음
    int id = model->GetModelID();
    modelsWithID[id] = model;
    return true;
}

void Engine::RequestLogAllStatus(){
    LOG_ORD ord;
    ord.ack_msg = "hello";
    std::any msg = ord;
    Event* event = new Event(this->rootModel->GetModelID(), "log_ord", msg);
    this->rootModel->ReceiveEvent(*event,this->currentTime);
}