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
    std::unordered_map<CouplingType, std::vector<Coupling*>> couplings;
    
public:
    std::unordered_map<int, Model*> modelsWithID;
    CoupledModel(int modelID, Engine* engine);

    const int GetComponentSize() const;    
    //
    bool AddCoupling(Model* srcModel, std::string srcPort, Model* detModel, std::string detPort, CouplingType type);
    bool RemoveCoupling(Model* srcModel, std::string* srcPort, Model* detModel, std::string* detPort);
    bool RemoveCoupling(Model* srcModel, std::string* srcPort);

    void ReceiveExternalEvent(const Event& externalEvent, TIME_T engineTime);
    void ReceiveTimeAdvanceRequest(const TIME_T engineTime);
    const TIME_T QueryNextTime() const; // TODO : 쿼리 올때마다 트리탐색은 과함, 그냥 engine에서 관리되는 atomic model에게만 query해도 무방한지 확인필요

    bool RegisterModelWithID(Model* model);
};