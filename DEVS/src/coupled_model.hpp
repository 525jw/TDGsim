#pragma once
#include "model.hpp"
#include "coupling.hpp"
#include <vector>
#include <unordered_map>
#include <string>

class CoupledModel : public Model{
private:
    std::vector<Model> subModels;
    std::vector<Coupling> couplings;
    
public:
    CoupledModel(int modelID);

    bool AddComponent(Model model);

    bool AddCoupling(Model* srcModel, std::string* srcPort, Model* detModel, std::string detPort);
    bool RemoveCoupling(Model* srcModel, std::string* srcPort, Model* detModel, std::string detPort);
    bool RemoveCoupling(Model* srcModel, std::string* srcPort);

    void HandleExtEvent(const Event extEvent, const std::string inPort, TIME_T engineTime);
    void HandleTimeAdvance(const TIME_T engineTime);
    const TIME_T queryNextTime() const;
};