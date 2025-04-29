#include "basic_model.h"
#include <algorithm>

void BasicModel::AddInputPort(const std::string& input){
    inputEvents.push_back(input);
}
void BasicModel::AddOutputPort(const std::string& output){
    outputEvents.push_back(output);
}
void BasicModel::RemoveInputPort(const std::string& input){
    inputEvents.erase(std::remove(inputEvents.begin(), inputEvents.end(), input), inputEvents.end());
}
void BasicModel::RemoveOutputPort(const std::string& output){
    outputEvents.erase(std::remove(outputEvents.begin(), outputEvents.end(), output), outputEvents.end());
}
void BasicModel::AddState(const std::string& state){
    states.push_back(state);
}
void BasicModel::RemoveState(const std::string& state){
    states.erase(std::remove(states.begin(), states.end(), state), states.end());
}
void BasicModel::SetModelID(int id){
    modelID=id;
}

const std::vector<std::string>& BasicModel::GetInputEvents() const{
    return inputEvents;
}
const std::vector<std::string>& BasicModel::GetOutputEvents() const{
    return outputEvents;
}
const std::vector<std::string>& BasicModel::GetStates() const{
    return states;
}
int BasicModel::GetModelID() const{
    return modelID;
}