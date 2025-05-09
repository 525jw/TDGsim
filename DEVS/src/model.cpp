#include "model.h"
#include <algorithm>

void Model::AddInputPort(const std::string& input){
    inputs.push_back(input);
}
void Model::AddOutputPort(const std::string& output){
    outputs.push_back(output);
}
void Model::RemoveInputPort(const std::string& input){
    inputs.erase(std::remove(inputs.begin(), inputs.end(), input), inputs.end());
}
void Model::RemoveOutputPort(const std::string& output){
    outputs.erase(std::remove(outputs.begin(), outputs.end(), output), outputs.end());
}

// 상태 관리
void Model::AddState(const std::string& state, const std::any& value){
    states[state] = value;
}
void Model::RemoveState(const std::string& state){
    states.erase(state);
}
void Model::SetStateValue(const std::string& state, const std::any& value){
    states[state] = value;
}
std::any Model::GetStateValue(const std::string& state) const{
    auto it = states.find(state);
    if (it != states.end()) return it->second;
    return {}; // Return an empty std::any if the state is not found
}


void Model::SetParentModel(Model* model){
    parentModel=model;
}
const Model* Model::GetParentModel() const{
    return parentModel;
}
void Model::SetModelID(int id){
    modelID=id;
}
int Model::GetModelID() const{
    return modelID;
}


const std::vector<std::string>& Model::GetInputEvents() const{
    return inputs;
}
const std::vector<std::string>& Model::GetOutputEvents() const{
    return outputs;
}
const std::unordered_map<std::string, std::any>& Model::GetStates() const{
    return states;
}


void Model::SetEngine(Engine* e) {
    engine = e;
}
Engine* Model::GetEngine() const {
    return engine;
}