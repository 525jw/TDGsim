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
void CoupledModel::AddCoupling(Model* src, const std::string& srcPort, Model* dst, const std::string& dstPort) {
    couplings.emplace_back(src, srcPort, dst, dstPort);
}

void CoupledModel::AddExternallInputPort(const std::string& inPort, Model* dstModel, const std::string& dstPort) {
    // 외부 입력 포트 추가
    AddCoupling(this, inPort, dstModel, dstPort);
}
void CoupledModel::AddExternallOutputPort(Model* srcModel, const std::string& srcPort, const std::string& outPort) {
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



void CoupledModel::BroadcastEvent(const Event& e) {
    for (const auto& edge : couplings) {
        if (edge.sourceModel == e.getSenderModel() && edge.sourcePort == e.getSenderPort()) {
            edge.targetModel->HandleExtEvent(e, edge.targetPort, e.getEventTime());
        }
    }
}

std::vector<Model*>& CoupledModel::GetSubModels() {
    return subModels;
}
const std::vector<Coupling>& CoupledModel::GetCouplingEdges() const {
    return couplings;
}