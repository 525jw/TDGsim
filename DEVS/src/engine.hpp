#pragma once
#include "coupled_model.hpp"
#include "event.hpp"
#include "logger.hpp"
#include "message.hpp"
#include <queue>
#include <vector>
#include <unordered_map>
#include <iostream>

class Engine {
private:
    Model* rootModel;
    std::unordered_map<int, Model*> modelsWithID; // NOTE : engine의 RegisterMoelWithID와 코드 중복 (modelWithID 구조 동일)
    std::queue<Event*> eventQueue;
    TIME_T currentTime;
public:
    Engine();

    void BuildDEVS(Model* rootModel);
    void Run();
    void AddEvent(Event* event); // NOTE : The only method accessible from external classes (e.g., AtomicModel) + Run would be public too, used in main.cpp
    bool RegisterModelWithID(Model* model);

    void RequestLogAllStatus();
};