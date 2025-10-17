#include "model.hpp"
#include "engine.hpp"
#include <utility>

Model::Model(Engine* engine, std::optional<std::string> name)
    : engine(engine), name(std::move(name))
{
    if (engine != nullptr) {
        engine->RegisterModelInEngine(this);
    }
}
void Model::SetModelID(int id){
    this->modelID=id;
}
void Model::SetModelName(std::optional<std::string> name){
    this->name = std::move(name);
}
void Model::SetEngine(Engine* engine) { 
    this->engine = engine; 
}
void Model::SetParentModel(Model* parentModel){
    this->parentModel = parentModel;
}
const int Model::GetModelID() const{
    return this->modelID;
}
const std::string& Model::GetName() const {
    static const std::string kDefaultName{"NONAME"};
    return this->name ? *this->name : kDefaultName;
}
std::string Model::GetNameWithId() const {
    return this->GetName()+ "(" + std::to_string(this->GetModelID()) + ")";
}
const Engine* Model::GetEngine() const{
    return this->engine;
}
Model* Model::GetParentModel() const{
    return this->parentModel;
}
const TIME_T Model::GetNextTime() const{
    return this->nextTime;
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
Model::~Model() = default;
