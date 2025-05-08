#pragma once
#include "engine.hpp"
#include <queue>

Engine::Engine() 
    : couplingGraph(this), currentTime(0), rootModel(nullptr) {}

void Engine::BuildCouplingGraph(Model* model){
    this->rootModel = model;
    subModels.push_back(rootModel);
    // std::queue<Model*> modelBFS; // Traversal Queue
    // Model* currentModel = nullptr;
    // while(!modelBFS.empty()){
    //     currentModel = modelBFS.front(); 
    //     modelBFS.pop();
    //     if(!currentModel->IsAtomic()){
            
    //     }
    // }
    this->couplingGraph.AddNode();
    this->couplingGraph.AddEdge();
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