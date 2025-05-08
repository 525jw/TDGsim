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
                        
    return false;
}
bool CoupledModel::RemoveCoupling(Model* srcModel, std::string* srcPort) {

    return false;
}

void CoupledModel::HandleExtEvent(const Event extEvent, const std::string inPort, TIME_T engineTime){
    // powergridsim 따라가면 필요없음
}
void CoupledModel::HandleTimeAdvance(const TIME_T engineTime){
    this->time = engineTime;

}
const TIME_T CoupledModel::queryNextTime() const{

}

// const bool Model::IsAtomic() const{
//     return false;
// }