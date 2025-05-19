#pragma once
#include "engine.hpp"
#include "logger.hpp"
#include <queue>
#include <iostream>

Engine::Engine() 
    : engineTime(0.0) {}

void Engine::BuildDEVS(Model* rootModel){
    this->rootModel = rootModel;
}

void Engine::Run(){
    

    std::vector<int> keys;
    for (const auto& pair : modelsWithID) {
        keys.push_back(pair.first);
    }
    for (const auto& key : keys) {
        logger << "Engine has Model ID: " << key << std::endl;
    }

    while(this->engineTime<10.0){ //// TODO : needs while loop unitl the end time

        logger<<"[Engine] engine run"<<std::endl;
        logger<<"[Engine] current time = "<<this->engineTime<<" event queue size = "<<this->eventQueue.size()<<std::endl;

        if(this->eventQueue.empty()){
            logger<<"[Engine] event queue is empty, query nextTime"<<std::endl;
            TIME_T minTA = this->rootModel->QueryNextTime();
            if(minTA > TIME_INF)
                return;
            this->engineTime = minTA;
            logger<<"[Engine] received minTA = "<<minTA<<" , request (*,"<<this->engineTime<<")"<<std::endl;
            this->rootModel->ReceiveTimeAdvanceRequest(this->engineTime);
        }else{
            logger<<"[Engine] starts broadcasting events"<<std::endl;
            Event* currentEvent = nullptr;
            Model* curModel = nullptr;
            while(!this->eventQueue.empty()){
                currentEvent = this->eventQueue.front(); this->eventQueue.pop();
                if (currentEvent == nullptr) {
                    logger << "[ERROR] currentEvent is nullptr!"<<std::endl;
                    continue;
                }
                logger<<"current Event starts from "<<currentEvent->getSenderModel()->GetModelID()<<std::endl;
                curModel = this->modelsWithID[currentEvent->getSenderModel()->GetModelID()];
                if (curModel == nullptr) {
                    logger << "[ERROR] currentEvent->getSenderModel() is nullptr!"<<std::endl;
                    continue;
                }
                logger<<"[Engine] request (x,"<<this->engineTime<<")"<<std::endl;
                rootModel->ReceiveExternalEvent(*currentEvent, this->engineTime); // !!!TODO!!! : 기존처럼 curModel이 받으면 안됨!!!!!!!!! 
            }
        }
    }
}

void Engine::AddEvent(Event* event){
    logger<<"event in "<<event->getSenderModel()->GetModelID()<<std::endl;
    this->eventQueue.push(event);
}
bool Engine::RegisterModelWithID(Model* model) {
    // TODO : engine에서 등록할때 ID를 배정해줄지, ID 받을지 논의 후 결정, 현재는 id를 받음
    int id = model->GetModelID();
    modelsWithID[id] = model;
    return true;
}