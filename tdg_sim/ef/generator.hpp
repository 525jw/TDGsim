#pragma once
#include "DEVS/atomic_model.hpp"
#include "DEVS/logger.hpp"
#include "common.hpp"
#include "tdg_sim/sim/Environment/environment.hpp"
#include "tdg_sim/sim/simulation.hpp"

class Generator : public AtomicModel{
private:
    unsigned int seed_;
    Scenario    scenario_; // 생성된 시나리오, common.hpp 참조

    unsigned int GenerateRandomSeed();
    bool LoadScenarioFromJson(const std::string& path, Scenario& out);

public:
    Generator(Engine* engine);

    bool ExtTransFn(const std::string& inPort, const std::any& anyMessage) override;
    bool IntTransFn() override;
    bool OutputFn() override;
    float TimeAdvanceFn() override;
};