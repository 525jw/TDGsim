#include "transducer.hpp"

Transducer::Transducer(Engine* engine)
    :  AtomicModel(engine)
{
    this->AddState("WAIT");
    this->AddState("RESTART");

    this->SetCurState("WAIT");
    
    this->AddInputPort("Start");
    // this->AddOutputPort("Restart");

    this->UpdateTime(0.0f);
}

bool Transducer::ExtTransFn(const std::string& inPort, const std::any& anyMessage) {
    if(inPort == "Start" && this->GetCurState() == "WAIT"){
        StartMsg msg;
        if (!TryCastMessage(anyMessage, msg, "Transducer::ExtTransFn.Start")) return false;
        this->goalRects_ = msg.scen->goalRects;
        this->experimentIndex_ = msg.experimentIndex;
        this->SetCurState("RESTART");
    }
    return true;
}
bool Transducer::OutputFn() {
    if (this->GetCurState() == "RESTART") {
        LogSimulation(this->engine->GetCurrentTime(),this->GetName(),"SIM ENDS");
        // gather result data from Environment
        if (!this->ReadResultFromSim(this->result_)) {
            LogError(this->engine->GetCurrentTime(), this->GetName(), "Failed to read data from Environment.");
        }
        // write result to CSV - this might not obey DEVS rules strictly, it should be implemented by event transmission: 
        if (!this->StoreResultCSV(path::RESULT_CSV, this->result_)) {
            LogError(this->engine->GetCurrentTime(), this->GetName(), "Failed to store result to CSV.");
        }
        // EF 개념 공부하고 다시 구현 필요
        // RestartMsg message;
        // message.needChangeScenario = false;
        // message.needChangeSeed = true;
        // std::any anyMessage = message;
        // this->AddOutputEvent("Restart",anyMessage);
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
    if (this->GetCurState() == "RESTART") return this->engine->GetSimulationEndTime();
    if(this->GetCurState()=="WAIT") return TIME_INF;
    return -1;
}
bool Transducer::ReadResultFromSim(Result& result) {
    // 컬럼헤더 바뀌면 여기도 수정 필요
    // seed
    result.seed = env->GetSeed();
    std::pair<int,int> initRifleCounts = env->QueryInitialEntityCounts(ForceType::RIFLE);
    // blueInit
    result.blueInit = initRifleCounts.first;
    // redInit
    result.redInit  = initRifleCounts.second;
    // blueCasualties & redCasualties
    std::pair<int,int> aliveRifleCounts = env->QueryEntityCounts(ForceType::RIFLE);
    result.blueCasualties = result.blueInit - aliveRifleCounts.first;
    result.redCasualties  = result.redInit  - aliveRifleCounts.second;
    // bg control info & totalScore
    const size_t goalCount = this->goalRects_.size();
    result.goalBlueCount.clear();
    result.goalRedCount.clear();
    result.goalScore.clear();
    result.goalBlueCount.reserve(goalCount);
    result.goalRedCount.reserve(goalCount);
    result.goalScore.reserve(goalCount);
    result.totalScore = 0.0f;
    for (const auto& [scoreWeight, rect] : this->goalRects_) {
        const auto [blueCount, redCount] = env->QueryEntityCounts(ForceType::DEFAULT, rect);
        result.goalBlueCount.push_back(blueCount);
        result.goalRedCount.push_back(redCount);
        float goalScore = 0.0f;
        if (blueCount > 0) {
            goalScore = (redCount == 0) ? scoreWeight : (scoreWeight * 0.5f);
        }
        result.goalScore.push_back(goalScore);
        result.totalScore += goalScore;
    }
    return true;
}
// column headers: seed, blueInit, redInit, blueCasualties, redCasualties, bg control info(red/blue/score per goal), totalScore
bool Transducer::StoreResultCSV(const std::string_view& path, const Result& result) {
    const std::string filename(path);

    bool needsHeader = true; 
    {
        std::ifstream ifs(filename);
        if (ifs.is_open()) {
            std::string firstLine;
            if (std::getline(ifs, firstLine)) {
                needsHeader = firstLine.empty();
            }
        }
    }

    std::ios_base::openmode mode = std::ios::out | std::ios::app;
    std::ofstream ofs(filename, mode);
    if (!ofs.is_open()) {
        return false;
    }
    // 헤더
    if (needsHeader) {
        ofs << "expIndex"
            << ",seed"
            << ",blueInit"
            << ",redInit"
            << ",blueCasualties"
            << ",redCasualties";
        const size_t goalCount = result.goalScore.size();
        for (size_t i = 0; i < goalCount; ++i) {
            const size_t idx = i + 1;
            ofs << ",goal" << idx << "Red"
                << ",goal" << idx << "Blue"
                << ",goal" << idx << "Score";
        }
        ofs << ",totalScore\n";
    }
    // 데이터
    ofs << this->experimentIndex_
        << ',' << result.seed
        << ',' << result.blueInit
        << ',' << result.redInit
        << ',' << result.blueCasualties
        << ',' << result.redCasualties;
    const size_t goalCount = result.goalScore.size();
    for (size_t i = 0; i < goalCount; ++i) {
        ofs << ',' << result.goalRedCount[i]
            << ',' << result.goalBlueCount[i]
            << ',' << result.goalScore[i];
    }
    ofs << ',' << result.totalScore << '\n';
    return true;
}

