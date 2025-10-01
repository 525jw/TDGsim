#pragma once
#include "DEVS/atomic_model.hpp"
#include "common.hpp"
#include "DEVS/logger.hpp"
#include "SIM/Environment/environment.hpp"

class BlueHQ : public AtomicModel{
private:
    std::vector<int> *memberIds;

    TIME_T t_dec=0.0f;
public:
    BlueHQ(Engine* engine, std::vector<int> *membersId);

    bool ExtTransFn(const std::string& inPort, const std::any& anyMessage);
    bool IntTransFn();
    bool OutputFn();
    TIME_T TimeAdvanceFn();
};