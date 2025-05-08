#include "coupling.hpp"

Coupling::Coupling(Model* srcModel, std::string& srcPort, 
                   Model* detModel, std::string& detPort)
    : srcModel(srcModel), srcPort(srcPort), detModel(detModel), detPort(detPort) {}

const Model* Coupling::getSrcModel() const {
    return srcModel;
}

const std::string& Coupling::getSrcPort() const {
    return srcPort;
}

const Model* Coupling::getDetModel() const {
    return detModel;
}

const std::string& Coupling::getDetPort() const {
    return detPort;
}
