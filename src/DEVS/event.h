#pragma once
#include <string>

class Model;

class Event{
private:
    Model* sender;
    std::string eventName;
};