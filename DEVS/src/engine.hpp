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
    Graph couplingGraph;
public:
    Engine();

    void BuildCouplingGraph(Model* model);
    void Run();
    void AddEvent(Event event);
    const TIME_T GetCurrentTime() const;
};