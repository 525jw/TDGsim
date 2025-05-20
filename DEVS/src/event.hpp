#pragma once
#include <string>
#include <any>

class Event{
private:
    int senderModelID;
    std::string senderPort;
    const std::any message;

public:
    Event(int senderModelID, const std::string& senderPort, const std::any& message);
    
    const int getSenderModelID() const;
    const std::string& getSenderPort() const;
    const std::any& getMessage() const;

    void SetSenderModelID(int senderModelID);
    void SetSenderPort(const std::string& senderPort);
};