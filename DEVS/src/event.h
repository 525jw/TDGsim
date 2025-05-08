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
    
    float getEventTime() const;
    const Model* getSenderModel() const;
    const std::string& getSenderPort() const;
    const std::any& getMessage() const;
    
    bool operator<(const Event& other) const {
        return eventTime > other.eventTime; // For max-heap
    }
};