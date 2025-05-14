#include "coupling.hpp"

Coupling::Coupling(Model* srcModel, std::string& srcPort, 
                   Model* detModel, std::string& detPort)
    : srcModel(srcModel), srcPort(srcPort), detModel(detModel), detPort(detPort) {}

Model* Coupling::getSrcModel() {
    return srcModel;
}

std::string& Coupling::getSrcPort() {
    return srcPort;
}

Model* Coupling::getDetModel() {
    return detModel;
}

std::string& Coupling::getDetPort() {
    return detPort;
}
