#pragma once
#include "engine.hpp"
#include <queue>

Engine::Engine() 
    : engineTime(0.0) {}

void Engine::BuildDEVS(Model* rootModel){
    this->rootModel = rootModel;
}

void Engine::Run(){
    // TODO : needs while loop unitl the end time
    if(this->eventQueue.empty()){
        TIME_T minTA = this->rootModel->QueryNextTime();
        if(minTA > TIME_INF)
            return;
        this->engineTime = minTA;
        this->rootModel->ReceiveTimeAdvanceRequest(this->engineTime);
    }else{
        Event* currentEvent = nullptr;
        while(!this->eventQueue.empty()){
            *currentEvent = *this->eventQueue.front(); this->eventQueue.pop(); 
            // TODO : pointer 끼리 = 연산이라 검토 필요
            for (auto& sm : subModels){
                if(sm == currentEvent->getSenderModel()){
                    sm->ReceiveExternalEvent(*currentEvent, this->engineTime);
                }
            }
        }
    }
}

void Engine::AddEvent(Event* event){
    this->eventQueue.push(event);
}