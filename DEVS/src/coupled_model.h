#pragma once
#include "model.h"
#include <vector>

class CoupledModel : public Model{
private:
    std::vector<Model> subModels;
    //edge
    //node with ID
public:
    CoupledModel(int modelID);

    bool AddInPorts();
    bool AddOutPorts();
    bool AddComponents();
    bool AddCoupling();
    bool SetPriority();
};