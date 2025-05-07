#pragma once
#include "engine.hpp"

void Engine::SetOutMostModel(Model* model){
    
}

void Engine::Run(){
    if(this->eventQueue.empty()){
        TIME_T minTA = this->rootModel->queryNextTime();
        if(minTA > INF_FLOAT)
            return;
        this->currentTime = minTA;
        this->rootModel->HandleTimeAdvance(this->currentTime);
    }else{
        Event* event = nullptr;
        while(!this->eventQueue.empty()){
            *event = this->eventQueue.front();
            
        }
    }
}

void Engine::AddEvent(Event event){
    this->eventQueue.push(event);
}