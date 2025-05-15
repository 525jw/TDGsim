#pragma once
#include "engine.hpp"
#include <queue>
#include <iostream>

Engine::Engine() 
    : engineTime(0.0) {}

void Engine::BuildDEVS(Model* rootModel){
    this->rootModel = rootModel;
    rootModel->ReceiveTimeAdvanceRequest(this->engineTime);
}

void Engine::Run(){
    while(engineTime<30.0){ //// TODO : needs while loop unitl the end time

        std::cout<<"[Engine] engine run, current time = "<<this->engineTime<<"\n";

        if(this->eventQueue.empty()){
            std::cout<<"[Engine] query nextTime\n";
            TIME_T minTA = this->rootModel->QueryNextTime();
            if(minTA > TIME_INF)
                return;
            this->engineTime = minTA;
            std::cout<<"[Engine] received minTA = "<<minTA<<" , request (*,"<<this->engineTime<<") \n";
            this->rootModel->ReceiveTimeAdvanceRequest(this->engineTime);
        }else{
            Event* currentEvent = nullptr;
            while(!this->eventQueue.empty()){
                currentEvent = this->eventQueue.front(); this->eventQueue.pop();
                for (auto& sm : subModels){
                    if(sm == currentEvent->getSenderModel()){
                        std::cout<<"[Engine] request (x,"<<this->engineTime<<") \n";
                        sm->ReceiveExternalEvent(*currentEvent, this->engineTime);
                    }
                }
            }
        }
    }
    
}

void Engine::AddEvent(Event* event){
    this->eventQueue.push(event);
}