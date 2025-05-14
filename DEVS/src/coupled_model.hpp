#pragma once
#include "model.hpp"
#include "coupling.hpp"
#include <vector>
#include <unordered_map>
#include <string>

class CoupledModel : public Model{
private:
    std::vector<Model> subModels; // TODO : change to a hash table for searching ID -> which model?
    // Q : 하위모델을 모두 호출할거면 vector만으로 충분한거 아닌가? , powersim SimulationEngine.py line 27  
    std::vector<Coupling> couplings;
    
public:
    // CoupledModel(int modelID, Engine* engine);
    CoupledModel(Engine* engine);

    bool AddComponent(Model model);

    bool AddCoupling(Model* srcModel, std::string* srcPort, Model* detModel, std::string* detPort);
    bool RemoveCoupling(Model* srcModel, std::string* srcPort, Model* detModel, std::string* detPort);
    bool RemoveCoupling(Model* srcModel, std::string* srcPort);

    void ReceiveExternalEvent(const Event& externalEvent, TIME_T engineTime);
    void ReceiveTimeAdvanceRequest(const TIME_T engineTime);
    const TIME_T QueryNextTime() const;
};