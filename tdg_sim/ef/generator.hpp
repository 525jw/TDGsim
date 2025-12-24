#pragma once
#include "DEVS/atomic_model.hpp"
#include "DEVS/logger.hpp"
#include "tdg_sim/common.hpp"
#include "tdg_sim/sim/environment/environment.hpp"
#include "tdg_sim/sim/simulation.hpp"
#include "tdg_sim/path.hpp"
#include "tdg_sim/data_loader.hpp"

class Generator : public AtomicModel{
private:
    unsigned int seed_;
    Scenario    scenario_; // 생성된 시나리오, common.hpp 참조

    int experimentIdx_ = 0; // 몇 번째 실험인지

    unsigned int GenerateRandomSeed();
    int GetExperiementIndexFromCsv();
public:
    Generator(Engine* engine);

    bool ExtTransFn(const std::string& inPort, const std::any& anyMessage) override;
    bool IntTransFn() override;
    bool OutputFn() override;
    float TimeAdvanceFn() override;
};
