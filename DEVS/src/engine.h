#pragma once
#include "coupled_model.h"
//#include "graph.h"
#include "event.h"
#include <queue>

class Engine {
private:
    Model* rootModel;
    std::priority_queue<Event> eventQueue;
    float currentTime;
public:
    void SetOutMostModel(Model model);
    void Run();
    void AddEvent(Event event);
};