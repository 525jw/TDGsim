#include "event.hpp"

Event::Event(int senderModelID, const std::string& senderPort, const std::any& message)
    : senderModelID(senderModelID), senderPort(senderPort), message(message) {}

const int Event::getSenderModelID() const {
    return this->senderModelID;
}

const std::string& Event::getSenderPort() const {
    return this->senderPort;
}

const std::any& Event::getMessage() const {
    return this->message;
}

void Event::SetSenderModelID(int senderModelID) {
    this->senderModelID = senderModelID;
}

void Event::SetSenderPort(const std::string& senderPort) {
    this->senderPort = senderPort;
}
