#include "engine.hpp"

Engine::Engine() : currentTime(0.0f), lastTime(0.0f) {}

void Engine::Run(){
    while(this->currentTime<1000.0){ //// TODO : needs while loop unitl the end time
        LogTrace(this->GetCurrentTime(),"Engine::Run","event queue size=",this->eventQueue.size(),"-------------------------------------------------------------------");

        if(lastTime > currentTime){
            LogError(this->GetCurrentTime(),"Engine::Run","time regression"," last=",ToFixedString(lastTime)," current=",ToFixedString(currentTime));
            for (const auto& pair : modelsWithID) {
                if (pair.second == nullptr) continue;
                LogError(this->GetCurrentTime(),"Engine::Run"," model=",pair.second->GetNameWithId()," next=",pair.second->GetNextTime());
            }
            exit(1);
        }

        if(this->eventQueue.empty()){
            LogTrace(this->GetCurrentTime(),"Engine::Run","event queue is empty");


            TIME_T minTA = this->rootModel->QueryNextTime();
            if(minTA > TIME_INF)
                return;
            this->lastTime = this->currentTime;
            this->currentTime = minTA;

            LogTrace(this->GetCurrentTime(),"Engine::Run","received minTA=",minTA," requests (*,t)");
            this->rootModel->ReceiveScheduleTime(this->currentTime);
        }else{
            LogTrace(this->GetCurrentTime(),"Engine::Run","event queue is not empty");
            
            Event* curEvent = nullptr;
            Model* curModel = nullptr;
            while(!this->eventQueue.empty()){
                curEvent = this->eventQueue.front(); this->eventQueue.pop();

                if (curEvent == nullptr) {
                    std::cerr << "[ERROR] CurrentEvent is nullptr!"<<std::endl;
                    continue;
                }

                curModel = this->modelsWithID[curEvent->getSenderModelID()];
                if (curModel == nullptr) {
                    std::cerr << "[ERROR] SenderModel is nullptr!"<<std::endl;
                    continue;
                }

                LogTrace(this->GetCurrentTime(),"Engine::Run","requests (x,t)",
                        " senderModel=",this->modelsWithID[curEvent->getSenderModelID()]->GetNameWithId(),
                        " senderPort=",curEvent->getSenderPort());

                curModel->GetParentModel()->ReceiveEvent(*curEvent, this->currentTime);
            }
        }
    }
}

void Engine::AddEvent(Event* event){
    LogTrace(this->GetCurrentTime(),"Engine::AddEvent","event in",
            " senderModel=",this->modelsWithID[event->getSenderModelID()]->GetNameWithId()," senderPort=",event->getSenderPort());

    this->eventQueue.push(event);
}
int Engine::RegisterModelInEngine(Model* model) {
    if(!model) return -1;

    int id = model->GetModelID();
    if(id <= 0 || modelsWithID.count(id)){
        id = this->nextModelID++;
    }else if(modelsWithID[id] == model){
        return id;       
    }
    modelsWithID[id] = model;
    model->SetModelID(id);
    return id;
}
