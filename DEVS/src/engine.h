#pragma once
#include "coupled_model.h"
//#include "graph.h"
#include "event.h"
#include <queue>

class Engine {
private:
    std::priority_queue<Event> eventQueue;
    std::vector<Model*> models;
    float currentTime = 0.0f;
public:
    Engine();

    void AddModel(Model* model);
    void AddEvent(Event event);
    float GetCurrentTime() const;

    void Run();

    void Reset();
    //void SetOutMostModel(Model* model);
};