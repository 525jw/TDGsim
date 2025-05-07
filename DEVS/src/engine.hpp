#pragma once
#include "coupled_model.hpp"
#include "event.hpp"
#include <queue>

class Engine {
private:
    Model* rootModel;
    std::queue<Event> eventQueue;
    TIME_T currentTime;
public:
    void SetOutMostModel(Model* model);
    void Run();
    void AddEvent(Event event);
    const TIME_T GetCurrentTime() const;
};