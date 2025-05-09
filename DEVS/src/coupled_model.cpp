#include "coupled_model.h"
#include <iostream>

CoupledModel::CoupledModel(int modelID) {
    SetModelID(modelID);
}

void CoupledModel::AddModel(Model* model) {
    subModels.push_back(model);
    model->SetParentModel(this);
    model->SetEngine(engine); // 상속받은 엔진 주입
}
void CoupledModel::AddCoupling(Model* src, const std::string& srcPort, Model* tgt, const std::string& tgtPort) {
    couplings.emplace_back(src, srcPort, tgt, tgtPort);
}

void CoupledModel::AddExternalInputPort(const std::string& inPort, Model* dstModel, const std::string& dstPort) {
    // 외부 입력 포트 추가
    AddCoupling(this, inPort, dstModel, dstPort);
}
void CoupledModel::AddExternalOutputPort(Model* srcModel, const std::string& srcPort, const std::string& outPort) {
    // 외부 출력 포트 추가
    AddCoupling(srcModel, srcPort, this, outPort);
}


// CoupledModel은 외부 이벤트 처리 안 함
void CoupledModel::HandleExtEvent(const Event& extEvent, const std::string& port, float time) {
    BroadcastEvent(extEvent);
}
void CoupledModel::HandleTimeAdvance() {
    for (auto* m : subModels) {
        // 조건 추가도 가능 m->GetNextTime() <= currentTime
        m->HandleTimeAdvance();
    }
}


float CoupledModel::GetNextTimeAdvance() const {
    float minNextTimeAdvance = std::numeric_limits<float>::max();
    for (const auto* m : subModels) {
        float time = m->GetNextTimeAdvance();
        if (time < minNextTimeAdvance) {
            minNextTimeAdvance = time;
        }
    }
    return minNextTimeAdvance;
}
// 자식 모델들 중 가장 먼저 움직이는 시간
float CoupledModel::GetNextTime() const {
    float minNextTime = std::numeric_limits<float>::max();
    for (const auto* m : subModels) {
        float time = m->GetNextTime();
        if (time < minNextTime) {
            minNextTime = time;
        }
    }
    return minNextTime;
}

// 타겟은 coupling을 기준으로 동적으로 결정됨
void CoupledModel::BroadcastEvent(const Event& e) {
    for (const auto& edge : couplings) {
        if (edge.sourceModel == e.GetSenderModel() && edge.sourcePort == e.GetSenderPort()) {
            edge.targetModel->HandleExtEvent(e, edge.targetPort, e.GetEventTime());
        }
    }
}

std::vector<Model*>& CoupledModel::GetSubModels() {
    return subModels;
}
const std::vector<Coupling>& CoupledModel::GetCouplingEdges() const {
    return couplings;
}