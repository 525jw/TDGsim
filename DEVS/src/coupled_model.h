#pragma once
#include "model.h"
#include "coupling.h"
#include "event.h"
#include <vector>
#include <map>

class CoupledModel : public Model{
private:
    std::vector<Model*> subModels; // Changed to pointers to avoid slicing
    std::vector<Coupling> couplings; // List of couplings between sub-models
    // Additional can be added...

public:
    CoupledModel(int ModelID);

    // sub model 추가, 연결 추가
    void AddModel(Model* model);
    void AddCoupling(Model* src, const std::string& srcPort, Model* dst, const std::string& dstPort);

    void AddExternallInputPort(const std::string& inPort, Model* dstModel, const std::string& dstPort);
    void AddExternallOutputPort(Model* srcModel, const std::string& srcPort, const std::string& outPort);

    void HandleExtEvent(const Event& extEvent, const std::string& port, float time) override;
    void HandleTimeAdvance() override;

    // time getter
    float GetNextTimeAdvance() const;
    float GetNextTime() const;

    void BroadcastEvent(const Event& e);
    
    std::vector<Model*>& GetSubModels();
    const std::vector<Coupling>& GetCouplingEdges() const;
};