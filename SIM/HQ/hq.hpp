#pragma once
#include "DEVS/atomic_model.hpp"
#include "common.hpp"
#include "DEVS/logger.hpp"
#include "SIM/Environment/environment.hpp"

class HQ : public AtomicModel{
private:
    std::unordered_set<int> controlledEntityIds;
    Side hqSide;
    std::string ordersFilePath;
    TIME_T t_dec = 0.0f;
    TIME_T t_rep = 0.0f;
public:
    HQ(Engine* engine,
       std::vector<int>* membersId,
       Side side = Side::BLUE,
       std::string ordersFile = "output.json");

    bool ExtTransFn(const std::string& inPort, const std::any& anyMessage);
    bool IntTransFn();
    bool OutputFn();
    TIME_T TimeAdvanceFn();
};
