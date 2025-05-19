#pragma once
#include "coupled_model.hpp"
#include "event.hpp"
#include <queue>
#include <vector>
#include <unordered_map>

class Engine {
private:
    Model* rootModel;
    std::unordered_map<int, Model*> modelsWithID;
    std::queue<Event*> eventQueue;
    TIME_T engineTime;
public:
    Engine();

    void BuildDEVS(Model* rootModel);
    void Run();
    void AddEvent(Event* event); // The only method accessible from external classes (e.g., AtomicModel)
    bool RegisterModelWithID(Model* model);
};