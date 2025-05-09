#pragma once
#include <string>
#include <any>

class Model;

class Event{
private:
    Model* senderModel;
    std::string senderPort;
    std::any message;
    float eventTime;

public:
    Event(Model* senderModel, const std::string& senderPort, const std::any& message, float eventTime);
    
    float GetEventTime() const;
    const Model* GetSenderModel() const;
    const std::string& GetSenderPort() const;
    const std::any& GetMessage() const;
    
    bool operator<(const Event& other) const {
        return eventTime > other.eventTime; // For max-heap
    }
};