#include "engine.h"
#include <iostream>
#include <limits>
/*
void Engine::SetOutMostModel(Model* model) {
    rootModel = model;
}
*/

Engine::Engine() : currentTime(0.0f) {}

void Engine::AddModel(Model* model) {
    models.push_back(model);
    model->SetEngine(this);

    auto* coupledModel = dynamic_cast<CoupledModel*>(model);
    if (coupledModel) {
        for (auto* child : coupledModel->GetSubModels()) {
            child->SetEngine(this);
        }
    }
}

void Engine::AddEvent(Event event) {
    std::cout << "[Engine] Event scheduled: " 
    << event.GetSenderPort()
    << " @ " << event.GetEventTime() << std::endl;
    eventQueue.push(event);
}

float Engine::GetCurrentTime() const {
    return currentTime;
}

/*
    1. 초기화: currentTime = 0.0
    2. 이벤트가 있는 동안:
        1. 전체 모델 중 GetNextTime() 최소값 = nextTime
        2. currentTime = nextTime
        3. 모든 model 중 GetNextTime() == currentTime 인 모델만 HandleTimeAdvance() 실행
        4. 큐에서 currentTime 이전까지 도달한 외부 이벤트 처리
*/
void Engine::Run() {
    currentTime = 0.0f;

    while (!eventQueue.empty()) {
        Event ev = eventQueue.top(); eventQueue.pop();
        currentTime = ev.GetEventTime();
        
        Model* sender = const_cast<Model*>(ev.GetSenderModel());
        Model* parent = const_cast<Model*>(sender->GetParentModel());

        if (auto* coupledModel = dynamic_cast<CoupledModel*>(parent)) {
            // CoupledModel일 경우 broadcast 필요
            coupledModel->BroadcastEvent(ev);
        } else {
            // AtomicModel이라면 직접 처리
            sender->HandleExtEvent(ev, ev.GetSenderPort(), currentTime);
        }

        // 시뮬레이션 시각이 도달했다면 내부 시간전진 수행
        if (sender->GetNextTime() <= currentTime) {
            sender->HandleTimeAdvance();
        }

        // 출력 이벤트가 발생할 수 있으며, model->addOutputEvent()에서 처리됨
    }

    std::cout << "[Engine] Simulation finished at time: " << currentTime << std::endl;
}


void Engine::Reset() {
    while (!eventQueue.empty()) {
        eventQueue.pop();
    }
    models.clear();
    currentTime = 0.0f;
}