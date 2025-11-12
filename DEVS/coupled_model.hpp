#pragma once
#include "model.hpp"
#include "coupling.hpp"
#include "logger.hpp"
#include <vector>
#include <unordered_map>
#include <string>
#include <algorithm>
#include <iostream>
#include <memory>

enum CouplingType { EIC, EOC, IC };

class CoupledModel : public Model{
private:
    std::unordered_map<CouplingType, std::vector<std::unique_ptr<Coupling>>> couplings;
    void RouteEIC(Event& event, float currentTime); 
    void RouteEOC(Event& event, float currentTime);
    void RouteIC(Event& event, float currentTime);
    Event Translate(const Event& in, int srcModelID, const std::string& srcPort);
public:
    std::unordered_map<int, Model*> subModelsWithID;
    CoupledModel(Engine* engine, std::optional<std::string> name = std::nullopt);

    bool AddCoupling(Model* srcModel, const std::string& srcPort, Model* detModel, const std::string& detPort, CouplingType type);
    bool RemoveCoupling(Model* srcModel, std::string* srcPort, Model* detModel, std::string* detPort);
    bool RemoveCoupling(Model* srcModel, std::string* srcPort);
    
    void ReceiveEvent(Event& event, float currentTime);
    void ReceiveScheduleTime(const float currentTime);

    const float QueryNextTime() const;
    
    bool RegisterSubModel(Model* model);
    bool IsCoupled() const override { return true; }

    // DEBUG ONLY
    size_t CouplingCount(CouplingType t) const {
        auto it = couplings.find(t);
        return (it == couplings.end()) ? 0 : it->second.size();
    }
};
