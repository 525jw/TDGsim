#pragma once
#include "engine.h"
#include <iostream>

void Engine::SetOutMostModel(Model* model) {
    rootModel = model;
}

void Engine::AddEvent(Event event) {
    eventQueue.push(event);
}

void Engine::Run() {
    while (!eventQueue.empty()) {
        Event ev = eventQueue.top(); eventQueue.pop();
        currentTime = ev.getEventTime();

        Model* model = const_cast<Model*>(ev.getSenderModel());
        if (!model) continue;

        // CoupledModel일 경우 broadcast 필요
        // AtomicModel이라면 직접 처리
        model->HandleExtEvent(ev, ev.getSenderPort(), currentTime);

        // 시뮬레이션 시각이 도달했다면 내부 시간전진 수행
        model->HandleTimeAdvance();

        // 출력 이벤트가 발생할 수 있으며, model->addOutputEvent()에서 처리됨
    }

    std::cout << "[Engine] Simulation finished at time: " << currentTime << std::endl;
}
