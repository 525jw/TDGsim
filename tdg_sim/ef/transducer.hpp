#include "DEVS/atomic_model.hpp"
#include "DEVS/logger.hpp"
#include "tdg_sim/common.hpp"
#include "tdg_sim/sim/environment/environment.hpp"
#include "tdg_sim/sim/simulation.hpp"

class Transducer : public AtomicModel{
private:
    Result    result_;
    float simulationEndTime_ = 3600.0f; // 시뮬레이션 종료 시간 (초)

    bool ReadMapFromJSON(const std::string& path, nlohmann::json& j);
    bool ReadResultFromSimulation(Result& result);
    bool StoreResultToCSV(const std::string& path, const Result& result);
public:
    Transducer(Engine* engine);

    bool ExtTransFn(const std::string& inPort, const std::any& anyMessage) override;
    bool IntTransFn() override;
    bool OutputFn() override;
    float TimeAdvanceFn() override;
};