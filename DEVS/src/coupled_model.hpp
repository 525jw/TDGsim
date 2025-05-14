#pragma once
#include "model.hpp"
#include "coupling.hpp"
#include <vector>
#include <unordered_map>
#include <string>
#include <iostream>

enum CouplingType { EIC, EOC, IC };

class CoupledModel : public Model{
private:
    std::vector<Model> subModels;
    std::unordered_map<CouplingType, std::vector<Coupling>> couplings;
    
public:
    // CoupledModel(int modelID, Engine* engine);
    CoupledModel(Engine* engine);

    bool AddComponent(Model model);

    bool AddCoupling(Model* srcModel, std::string* srcPort, Model* detModel, std::string* detPort, CouplingType* type);
    bool RemoveCoupling(Model* srcModel, std::string* srcPort, Model* detModel, std::string* detPort);
    bool RemoveCoupling(Model* srcModel, std::string* srcPort);

    void ReceiveExternalEvent(const Event& externalEvent, TIME_T engineTime);
    void ReceiveTimeAdvanceRequest(const TIME_T engineTime);
    const TIME_T QueryNextTime() const;
};