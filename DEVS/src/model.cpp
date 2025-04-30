#pragma once
#include "model.h"
#include <algorithm>

void Model::AddInputPort(const std::string& input){
    inputEvents.push_back(input);
}
void Model::AddOutputPort(const std::string& output){
    outputEvents.push_back(output);
}
void Model::RemoveInputPort(const std::string& input){
    inputEvents.erase(std::remove(inputEvents.begin(), inputEvents.end(), input), inputEvents.end());
}
void Model::RemoveOutputPort(const std::string& output){
    outputEvents.erase(std::remove(outputEvents.begin(), outputEvents.end(), output), outputEvents.end());
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
    return inputEvents;
}
const std::vector<std::string>& Model::GetOutputEvents() const{
    return outputEvents;
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