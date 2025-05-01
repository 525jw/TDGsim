#pragma once
#include <vector>
#include <string>

class Model{
private:
    std::vector<std::string> inputs;
    std::vector<std::string> outputs;
    std::vector<std::string> states;
    Model* parentModel = nullptr;
    int modelID = 0;
      

public:

    void AddInputPort(const std::string& input);
    void AddOutputPort(const std::string& output);
    void AddState(const std::string& state);
    void RemoveInputPort(const std::string& input);
    void RemoveOutputPort(const std::string& output);
    void RemoveState(const std::string& state);
    void SetParentModel(Model* model);
    void SetModelID(int id);

    const std::vector<std::string>& GetInputEvents() const;
    const std::vector<std::string>& GetOutputEvents() const;
    const std::vector<std::string>& GetStates() const;
    const Model* GetParentModel() const;
    const int GetModelID() const;
};