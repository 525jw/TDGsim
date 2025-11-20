#pragma once
#include "DEVS/atomic_model.hpp"
#include "common.hpp"
#include "DEVS/logger.hpp"
#include "SIM/Environment/environment.hpp"

class HQ : public AtomicModel{
private:
    std::unordered_set<int> memberIds; // 자식 노드에 해당하는 id만 보유
    SideType hqSide;
    std::string ordersFilePath;
    float t_dec = 0.0f;
    float t_rep = 0.0f;
public:
    HQ(Engine* engine,
       std::vector<int>* membersId,
       Side side = SideType::BLUE,
       std::string ordersFile = "output.json");

    bool ExtTransFn(const std::string& inPort, const std::any& anyMessage);
    bool IntTransFn();
    bool OutputFn();
    float TimeAdvanceFn();
};
