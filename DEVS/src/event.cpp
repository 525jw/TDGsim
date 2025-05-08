#include "event.h"
#include "model.h"

Event::Event(Model* senderModel, const std::string& senderPort, const std::any& message, float eventTime)
    : senderModel(senderModel), senderPort(senderPort), message(message), eventTime(eventTime) {}

const Model* Event::getSenderModel() const {
    return senderModel;
}

const std::string& Event::getSenderPort() const {
    return senderPort;
}

const std::any& Event::getMessage() const {
    return message;
}

float Event::getEventTime() const {
    return eventTime;
}

const Model* Event::getSenderModel() const {
    return senderModel;
}