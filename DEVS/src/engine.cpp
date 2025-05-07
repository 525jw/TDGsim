#pragma once
#include "engine.hpp"

Engine::Engine() 
    : couplingGraph(this), currentTime(0), rootModel(nullptr) {}

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
            this->eventQueue.pop();
            this->couplingGraph.broadcastEvent(*event);
        }
    }
}

void Engine::AddEvent(Event event){
    this->eventQueue.push(event);
}