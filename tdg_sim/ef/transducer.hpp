#include "DEVS/atomic_model.hpp"
#include "DEVS/logger.hpp"
#include "tdg_sim/common.hpp"
#include "tdg_sim/sim/environment/environment.hpp"
#include "tdg_sim/sim/simulation.hpp"

class Transducer : public AtomicModel{
private:
    std::string resultPath_ = "data/result.csv"; // 결과 파일 경로
    Result    result_;
    float simulationEndTime_ = 3600.0f; // 시뮬레이션 종료 시간 (초)

    bool StoreResultToCSV(const std::string& path, const Result& result);
public:
    Transducer(Engine* engine);

    bool ExtTransFn(const std::string& inPort, const std::any& anyMessage) override;
    bool IntTransFn() override;
    bool OutputFn() override;
    float TimeAdvanceFn() override;
};