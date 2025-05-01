#pragma once
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
void Model::AddState(const std::string& state){
    states.push_back(state);
}
void Model::RemoveState(const std::string& state){
    states.erase(std::remove(states.begin(), states.end(), state), states.end());
}
void Model::SetParentModel(Model* model){
    parentModel=model;
}
void Model::SetModelID(int id){
    modelID=id;
}

const std::vector<std::string>& Model::GetInputEvents() const{
    return inputs;
}
const std::vector<std::string>& Model::GetOutputEvents() const{
    return outputs;
}
const std::vector<std::string>& Model::GetStates() const{
    return states;
}
const Model* Model::GetParentModel() const{
    return parentModel;
}
const int Model::GetModelID() const{
    return modelID;
}