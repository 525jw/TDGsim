#include "transducer.hpp"

Transducer::Transducer(Engine* engine)
    :  AtomicModel(engine)
{
    this->AddState("WAIT");
    this->AddState("RESTART");

    this->SetCurState("WAIT");
    
    this->AddInputPort("Start");
    this->AddOutputPort("Restart");

    this->UpdateTime(0.0f);
}

bool Transducer::ExtTransFn(const std::string& inPort, const std::any& anyMessage) {
    if(inPort == "Start" && this->GetCurState() == "WAIT"){
        this->SetCurState("RESTART");
    }
    return true;
}
bool Transducer::OutputFn() {
    if (this->GetCurState() == "RESTART") {
        // gather result data from Environment
        if (!this->ReadResultFromSimulation(this->result_)) {
            LogError(this->engine->GetCurrentTime(), this->GetName(), "Failed to read data from Environment.");
        }
        // write result to CSV - this might not obey DEVS rules strictly, it should be implemented by event transmission: 
        if (!this->StoreResultToCSV(RESULT_PATH, this->result_)) {
            LogError(this->engine->GetCurrentTime(), this->GetName(), "Failed to store result to CSV.");
        }
        RestartMsg message;
        message.needChangeScenario = false;
        message.needChangeSeed = false;
        std::any anyMessage = message;
        this->AddOutputEvent("Restart",anyMessage);
    }
    return true;
}
bool Transducer::IntTransFn() {
    if(this->GetCurState()=="RESTART"){
        this->SetCurState("WAIT");
    }
    return true;
}

float Transducer::TimeAdvanceFn() {
    if (this->GetCurState() == "RESTART") return this->simulationEndTime_;
    if(this->GetCurState()=="WAIT") return TIME_INF;
    return -1;
}
bool Transducer::ReadResultFromSimulation(Result& result) {
    result.seed = env->GetSeed();
    std::pair<int,int> initRifleCounts = env->QueryInitialEntityCounts(ForceType::RIFLE);
    result.blueInit = initRifleCounts.first;
    result.redInit  = initRifleCounts.second;
    std::pair<int,int> aliveRifleCounts = env->QueryEntityCounts(ForceType::RIFLE);
    result.blueCasualties = result.blueInit - aliveRifleCounts.first;
    result.redCasualties  = result.redInit  - aliveRifleCounts.second;
    result.bgControl.clear();
    // target area를 MAP_PATH에서 읽어오기

    result.bgControl[1] = env->QueryEntityCounts(ForceType::RIFLE);
    return true;
}
// column headers: seed, blueInit, redInit, blueCasualties, redCasualties, bg control info, totalScore
bool Transducer::StoreResultToCSV(const std::string& path, const Result& result) {
    std::ofstream ofs(path, std::ios::app);
    if (!ofs.is_open()) {
        LogError(this->engine->GetCurrentTime(), this->GetName(), "Failed to open result file:", path);
        return false;
    }
    ofs.close();
    return true;
}