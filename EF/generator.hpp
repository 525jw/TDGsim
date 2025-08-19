#pragma once
#include "DEVS/atomic_model.hpp"
#include "DEVS/logger.hpp"
#include "message.hpp"

class Generator : public AtomicModel{
private:
    unsigned int seed = 0;
    unsigned int GenerateSeed();
public:
    Generator(Engine* engine);
    bool ExtTransFn(const std::string& inPort, const std::any& anyMessage);
    bool IntTransFn();
    bool OutputFn();
    TIME_T TimeAdvanceFn();
};