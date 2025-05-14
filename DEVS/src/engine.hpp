#pragma once
#include "coupled_model.hpp"
#include "event.hpp"
#include "graph.hpp"
#include <queue>
#include <vector>

class Engine {
private:
    Model* rootModel;
    std::vector<Model*> subModels;
    std::queue<Event> eventQueue;
    TIME_T currentTime;
public:
    Engine();

    void BuildDEVS(Model* rootModel);

    void Run();
    void AddEvent(Event event); // The only method accessible from external classes (e.g., AtomicModel)
    const TIME_T GetCurrentTime() const;
};