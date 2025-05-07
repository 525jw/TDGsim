#include "event.hpp"

Event::Event(const Model& senderModel, const std::string& senderPort, const std::any& message)
    : senderModel(&senderModel), senderPort(senderPort), message(message) {}

const Model* Event::getSenderModel() const {
    return senderModel;
}

const std::string& Event::getSenderPort() const {
    return senderPort;
}

const std::any& Event::getMessage() const {
    return message;
}
