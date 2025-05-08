#pragma once
#include "model.hpp"

class Coupling{
    private:
        Model* srcModel;
        std::string srcPort;
        Model* detModel;
        std::string detPort;
    
    public:
        Coupling(Model* srcModel, std::string& srcPort, 
                 Model* detModel, std::string& detPort);
    
        const Model* getSrcModel() const;
        const std::string& getSrcPort() const;
        const Model* getDetModel() const;
        const std::string& getDetPort() const;
    };