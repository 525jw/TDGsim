#pragma once
#include "model.hpp"
#include "engine.hpp"
#include <algorithm>

void Model::AddInputPort(const std::string& input){
    this->inputs.push_back(input);
}
void Model::AddOutputPort(const std::string& output){
    this->outputs.push_back(output);
}
void Model::RemoveInputPort(const std::string& input){
    this->inputs.erase(std::remove(this->inputs.begin(), this->inputs.end(), input), this->inputs.end());
}
void Model::RemoveOutputPort(const std::string& output){
    this->outputs.erase(std::remove(this->outputs.begin(), this->outputs.end(), output), this->outputs.end());
}
void Model::AddState(const std::string& state){
    this->states.push_back(state);
}
void Model::RemoveState(const std::string& state){
    this->states.erase(std::remove(this->states.begin(), this->states.end(), state), this->states.end());
}
void Model::SetParentModel(Model& model){
    this->parentModel = &model;
}
void Model::SetModelID(int id){
    this->modelID=id;
}
void Model::SetEngine(Engine& engine) { 
    this->engine = &engine; 
}
const std::vector<std::string>& Model::GetInputEvents() const{
    return this->inputs;
}
const std::vector<std::string>& Model::GetOutputEvents() const{
    return this->outputs;
}
const std::vector<std::string>& Model::GetStates() const{
    return this->states;
}
const Model* Model::GetParentModel() const{
    return this->parentModel;
}
const int Model::GetModelID() const{
    return this->modelID;
}
const Engine* Model::GetEngine() const{
    return this->engine;
}