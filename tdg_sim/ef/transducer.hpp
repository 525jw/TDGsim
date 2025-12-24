#include "DEVS/atomic_model.hpp"
#include "DEVS/logger.hpp"
#include "tdg_sim/common.hpp"
#include "tdg_sim/sim/environment/environment.hpp"
#include "tdg_sim/sim/simulation.hpp"
#include "tdg_sim/path.hpp"

class Transducer : public AtomicModel{
private:
    Result    result_;
    std::vector<std::pair<float, Rect>> goalRects_;

    int experimentIndex_ = -1; // 몇 번째 실험인지

    bool ReadResultFromSim(Result& result);
    bool StoreResultCSV(const std::string_view& path, const Result& result);
public:
    Transducer(Engine* engine);

    bool ExtTransFn(const std::string& inPort, const std::any& anyMessage) override;
    bool IntTransFn() override;
    bool OutputFn() override;
    float TimeAdvanceFn() override;
};