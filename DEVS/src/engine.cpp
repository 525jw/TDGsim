#pragma once
#include "engine.hpp"
#include <queue>

Engine::Engine() 
    : currentTime(0.0) {}

void Engine::BuildDEVS(Model* rootModel){
    this->rootModel = rootModel;
}

void Engine::Run(){
    if(this->eventQueue.empty()){
        TIME_T minTA = this->rootModel->QueryNextTime();
        if(minTA > TIME_INF)
            return;
        this->currentTime = minTA;
        this->rootModel->ReceiveTimeAdvanceRequest(this->currentTime);
    }else{
        Event* currentEvent = nullptr;
        while(!this->eventQueue.empty()){
            *currentEvent = this->eventQueue.front(); this->eventQueue.pop();
            for (auto& sm : subModels){
                if(sm == currentEvent->getSenderModel()){
                    sm->ReceiveExternalEvent(*currentEvent, this->currentTime);
                }
            }
        }
    }
}

void Engine::AddEvent(Event event){
    this->eventQueue.push(event);
}