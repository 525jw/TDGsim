#pragma once
#include "coupled_model.hpp"
#include "event.hpp"
#include "logger.hpp"
#include <queue>
#include <vector>
#include <unordered_map>
#include <iostream>

class Engine {
private:
    Model* rootModel;
    
    std::queue<Event*> eventQueue;
    float currentTime;
    float lastTime;

    std::unordered_map<int, Model*> modelsWithID; // ID ascending from 1
    int  nextModelID = 1;
public:
    Engine();

    float GetCurrentTime() { return currentTime; };
    void SetRootModel(Model* model){ this->rootModel = model; };
    void Run();
    void AddEvent(Event* event);
    int RegisterModelInEngine(Model* model); // returns registered modelID, or -1 on error
    void RequestEndSimulation(){ this->currentTime = TIME_INF;} // is it safe?
    void RequestResetEngine();
};