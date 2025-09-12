#pragma once
#include "DEVS/atomic_model.hpp"
#include "message.hpp"
#include "DEVS/logger.hpp"
#include "SIM/Environment/environment.hpp"
#include <utility>
#include <iostream>
#include <string>

class BlueHQ : public AtomicModel{
private:
    TIME_T t_dec=0.0f;
public:
    BlueHQ(Engine* engine);

    bool ExtTransFn(const std::string& inPort, const std::any& anyMessage);
    bool IntTransFn();
    bool OutputFn();
    TIME_T TimeAdvanceFn();
};