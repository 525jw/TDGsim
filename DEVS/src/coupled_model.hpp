#pragma once
#include "model.hpp"
#include "coupling.hpp"
#include <vector>
#include <unordered_map>
#include <string>
#include <algorithm>
#include <iostream>

enum CouplingType { EIC, EOC, IC };

class CoupledModel : public Model{
private:
    std::unordered_map<CouplingType, std::vector<Coupling*>> couplings;
    
public:
    std::unordered_map<int, Model*> modelsWithID;
    CoupledModel(int modelID, Engine* engine);

    bool AddCoupling(Model* srcModel, std::string srcPort, Model* detModel, std::string detPort, CouplingType type);
    bool RemoveCoupling(Model* srcModel, std::string* srcPort, Model* detModel, std::string* detPort);
    bool RemoveCoupling(Model* srcModel, std::string* srcPort);

    
    void ReceiveEvent(Event& event, TIME_T currentTime);
    void RouteEIC(Event& event, TIME_T currentTime);
    void RouteEOC(Event& event, TIME_T currentTime);
    void RouteIC(Event& event, TIME_T currentTime);

    void ReceiveScheduleTime(const TIME_T engineTime);

    void Translate(Event& event, int srcModelID, std::string& srcPort);

    // const TIME_T QueryNextTime() const;
    bool RegisterModelWithID(Model* model);
    bool IsCoupled() const override { return true; }
};