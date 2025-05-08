#include "coupled_model.hpp"
#include "event.hpp"

CoupledModel::CoupledModel(int modelID) {
    SetModelID(modelID);
}

bool CoupledModel::AddComponent(Model model) {
    subModels.push_back(model);
    return true;
}

bool CoupledModel::AddCoupling(Model* srcModel, std::string* srcPort, 
                               Model* detModel, std::string detPort) {
    std::string* detPortPtr = new std::string(detPort);
    couplings.emplace_back(srcModel, srcPort, detModel, detPortPtr);
    return true;
}
bool CoupledModel::RemoveCoupling(Model* srcModel, std::string* srcPort,
                                  Model* detModel, std::string detPort) {
    auto it = std::remove_if(couplings.begin(), couplings.end(),
        [&](const Coupling& c) {
            return c.getSrcModel() == srcModel &&
                    *c.getSrcPort() == *srcPort &&
                    c.getDetModel() == detModel &&
                    *c.getDetPort() == detPort;
        });
    if (it != couplings.end()) {
        couplings.erase(it, couplings.end());
        return true;
    }
    return false;
}
bool CoupledModel::RemoveCoupling(Model* srcModel, std::string* srcPort) {
    auto it = std::remove_if(couplings.begin(), couplings.end(),
        [&](const Coupling& c) {
            return c.getSrcModel() == srcModel && *c.getSrcPort() == *srcPort;
        });
    if (it != couplings.end()) {
        couplings.erase(it, couplings.end());
        return true;
    }
    return false;
}

void CoupledModel::HandleExtEvent(const Event extEvent, const std::string inPort, TIME_T engineTime){

}
void CoupledModel::HandleTimeAdvance(const TIME_T engineTime){

}
const TIME_T CoupledModel::queryNextTime() const{

}

// const bool Model::IsAtomic() const{
//     return false;
// }