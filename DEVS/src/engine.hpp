#pragma once
#include "coupled_model.hpp"
#include "event.hpp"
#include <queue>
#include <vector>
#include <unordered_map>

class Engine {
private:
    Model* rootModel;
    std::unordered_map<int, Model*> modelsWithID; // NOTE : engine의 RegisterMoelWithID와 코드 중복 (modelWithID 구조 동일)
    std::queue<Event*> eventQueue;
    TIME_T engineTime;
public:
    Engine();

    void BuildDEVS(Model* rootModel);
    void Run();
    void AddEvent(Event* event); // NOTE : The only method accessible from external classes (e.g., AtomicModel) + Run would be public too, used in main.cpp
    bool RegisterModelWithID(Model* model);
};