#pragma once
#include "model.hpp"
#include <vector>
#include <string>

class CoupledModel : public Model{
private:
    std::vector<Model> subModels;
    //edge
    //node with ID
public:
    CoupledModel(int modelID);

    bool AddComponent(Model model);

    bool AddCoupling(CoupledModel *src_model, const std::string *src_port, CoupledModel *det_model, const std::string *det_port);
    bool RemoveCoupling(CoupledModel *src_model, const std::string *src_port, CoupledModel *det_model, const std::string *det_port);
    bool RemoveCoupling(CoupledModel *src_model, const std::string *src_port);

    void HandleExtEvent(const Event extEvent, const std::string inPort, TIME_T engineTime);
    void HandleTimeAdvance(const TIME_T engineTime);
    const TIME_T queryNextTime() const;
};