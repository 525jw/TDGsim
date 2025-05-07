#include "coupled_model.hpp"
#include "event.hpp"

CoupledModel::CoupledModel(int modelID){
    SetModelID(modelID);
}

bool CoupledModel::AddComponent(Model model){
    this->subModels.push_back(model);
}

bool CoupledModel::AddCoupling(CoupledModel *src_model, const std::string *src_port, CoupledModel *det_model, const std::string *det_port){
    
}
bool CoupledModel::RemoveCoupling(CoupledModel *src_model, const std::string *src_port, CoupledModel *det_model, const std::string *det_port){

}
bool CoupledModel::RemoveCoupling(CoupledModel *src_model, const std::string *src_port){

}

void CoupledModel::HandleExtEvent(const Event extEvent, const std::string inPort, TIME_T engineTime){

}
void CoupledModel::HandleTimeAdvance(const TIME_T engineTime){

}
const TIME_T CoupledModel::queryNextTime() const{

}