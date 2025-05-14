#pragma once
#include <string>
#include <any>

class Model;

class Event{
private:
    const Model* senderModel;
    std::string senderPort;
    std::any message;

public:
    Event(const Model& senderModel, const std::string& senderPort, const std::any& message);
    
    const Model* getSenderModel() const;
    const std::string& getSenderPort() const;
    const std::any& getMessage() const;
};