#pragma once
#include <string>
#include <any>

class Event{
private:
    std::any senderModel;
    std::string senderPort;
    std::any message;

public:
    Event(const std::any& senderModel, const std::string& senderPort, const std::any& message);
    
    const std::any& getSenderModel() const;
    const std::string& getSenderPort() const;
    const std::any& getMessage() const;
};