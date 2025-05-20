#pragma once
#include "coupled_model.hpp"
#include "event.hpp"
#include <queue>
#include <vector>
#include <unordered_map>

class Engine {
private:
    Model* rootModel;
    std::unordered_map<int, Model*> modelsWithID; // TODO : coupled model 멤버 변수와 같은 구조, 통합이 가능한지 논의 필요
    std::queue<Event*> eventQueue;
    TIME_T engineTime;
public:
    Engine();

    void BuildDEVS(Model* rootModel);
    void Run();
    void AddEvent(Event* event); // The only method accessible from external classes (e.g., AtomicModel)
    bool RegisterModelWithID(Model* model);
};