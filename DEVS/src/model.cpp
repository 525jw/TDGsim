#pragma once
#include "model.hpp"
#include "engine.hpp"
#include <algorithm>

// void Model::SetModelID(int id){
//     this->modelID=id;
// }
void Model::SetEngine(Engine* engine) { 
    this->engine = engine; 
}
// const int Model::GetModelID() const{
//     return this->modelID;
// }
const Engine* Model::GetEngine() const{
    return this->engine;
}

void Model::AddInputPort(const std::string& inputPort){
    this->inputPorts.push_back(inputPort);
}
void Model::AddOutputPort(const std::string& outputPort){
    this->outputPorts.push_back(outputPort);
}
void Model::RemoveInputPort(const std::string& inputPort){
    this->inputPorts.erase(std::remove(this->inputPorts.begin(), this->inputPorts.end(), inputPort), this->inputPorts.end());
}
void Model::RemoveOutputPort(const std::string& outputPort){
    this->outputPorts.erase(std::remove(this->outputPorts.begin(), this->outputPorts.end(), outputPort), this->outputPorts.end());
}
const std::vector<std::string>& Model::GetInputPorts() const{
    return this->inputPorts;
}
const std::vector<std::string>& Model::GetOutputPorts() const{
    return this->outputPorts;
}