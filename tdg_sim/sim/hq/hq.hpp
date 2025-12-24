#pragma once
#include "DEVS/atomic_model.hpp"
#include "DEVS/logger.hpp"
#include "tdg_sim/common.hpp"
#include "tdg_sim/sim/environment/environment.hpp"
#include "tdg_sim/path.hpp"

class HQ : public AtomicModel{
private:
    std::unordered_set<int> memberIds_;
    SideType side_; // HQ 소속 진영
    std::string_view bmlPath_;
    // devs
    float t_dec = 0.0f;
    float t_rep = 0.0f;
public:
    HQ(Engine* engine,
       std::vector<int>* membersId,
       SideType side = SideType::BLUE,
       std::string_view bmlPath = path::BML_JSON);

    bool ExtTransFn(const std::string& inPort, const std::any& anyMessage);
    bool IntTransFn();
    bool OutputFn();
    float TimeAdvanceFn();
};
