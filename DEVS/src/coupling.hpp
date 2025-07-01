#pragma once
#include <string>

class Model;

class Coupling {
private:
    Model* srcModel;
    std::string srcPort;
    Model* detModel;
    std::string detPort;
public:
    Coupling(Model* srcModel, const std::string& srcPort, Model* detModel, const std::string& detPort)
        : srcModel(srcModel), srcPort(srcPort), detModel(detModel), detPort(detPort) {}

    Model* getSrcModel() const { return srcModel; }
    const std::string& getSrcPort() const { return srcPort; }
    Model* getDetModel() const { return detModel; }
    const std::string& getDetPort() const { return detPort; }
};
