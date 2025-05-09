#include "event.h"
#include "model.h"

Event::Event(Model* senderModel, const std::string& senderPort, const std::any& message, float eventTime)
    : senderModel(senderModel), senderPort(senderPort), message(message), eventTime(eventTime) {}

const Model* Event::GetSenderModel() const {
    return senderModel;
}

const std::string& Event::GetSenderPort() const {
    return senderPort;
}
const std::any& Event::GetMessage() const {
    return message;
}

float Event::GetEventTime() const {
    return eventTime;
}