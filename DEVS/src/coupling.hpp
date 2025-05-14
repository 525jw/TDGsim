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
    
        Model* getSrcModel();
        std::string& getSrcPort();
        Model* getDetModel();
        std::string& getDetPort();
    };