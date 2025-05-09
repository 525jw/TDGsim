/*
#include "engine.h"
#include "sim_world.h"
#include <iostream>

int main(){
    SimWorld simWorld = SimWorld();
    Engine engine = Engine();

    engine.SetOutMostModel(&simWorld);
    engine.Run();
    
    return 0;
}
*/
#include "engine.h"
#include "coupled_model.h"
#include "dummy_model.h" // 위 헤더 포함

int main() {
    Engine engine;

    // 모델 생성
    auto* modelA = new DummyModel(1);
    auto* modelB = new DummyModel(2);

    // 상위 모델 구성
    auto* root = new CoupledModel(0);
    root->AddModel(modelA);
    root->AddModel(modelB);

    // A → B 연결
    root->AddCoupling(modelA, "out", modelB, "in");

    // 엔진에 CoupledModel 등록
    engine.AddModel(root);

    // 외부 이벤트 추가... start 버튼으로 구현할지..?
    engine.AddEvent(Event(modelA, "in", std::string("start"), 0.0f));

    // 시뮬레이션 실행
    engine.Run();

    // 정리
    delete modelA;
    delete modelB;
    delete root;

    return 0;
}
