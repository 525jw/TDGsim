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
    Coupling(Model* srcModel, std::string& srcPort, Model* detModel, std::string& detPort)
        : srcModel(srcModel), srcPort(srcPort), detModel(detModel), detPort(detPort) {}

    Model* getSrcModel() { return srcModel; }
    std::string& getSrcPort() { return srcPort; }
    Model* getDetModel() { return detModel; }
    std::string& getDetPort() { return detPort; }
};
