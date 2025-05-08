#pragma once
#include "model.h"
#include <string>

struct Coupling
{
    Model* sourceModel;
    std::string sourcePort;
    Model* targetModel;
    std::string targetPort;

    Coupling(Model* srcModel, const std::string& srcPort, Model* tgtModel, const std::string& tgtPort)
        : sourceModel(srcModel), sourcePort(srcPort), targetModel(tgtModel), targetPort(tgtPort) {}
};
