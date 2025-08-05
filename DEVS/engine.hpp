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
    TIME_T currentTime;
    TIME_T lastTime;

    std::unordered_map<int, Model*> modelsWithID; // ID ascending from 1
    int  nextModelID = 1;
public:
    Engine();

    void Run();
    void AddEvent(Event* event); // NOTE : The only method accessible from external classes (e.g., AtomicModel) + Run would be public too, used in main.cpp
    int RegisterModelWithID(Model* model); // returns registered modelID, or -1 on error
};