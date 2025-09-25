#pragma once
#include "DEVS/atomic_model.hpp"
#include "DEVS/logger.hpp"
#include "common.hpp"
#include "SIM/Environment/environment.hpp"
#include "SIM/simulation.hpp"

class Generator : public AtomicModel{
private:
    unsigned int randomSeed_;
    std::string scenarioPath_;
    Scenario    scenario_;

    unsigned int GenerateRandomSeed();
    bool LoadScenarioFromJson(const std::string& path, Scenario& out); // nlohmann::json 사용

public:
    Generator(Engine* engine);

    bool ExtTransFn(const std::string& inPort, const std::any& anyMessage) override;
    bool IntTransFn() override;
    bool OutputFn() override;
    TIME_T TimeAdvanceFn() override;
};