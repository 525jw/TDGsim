#include "transducer.hpp"

Transducer::Transducer(Engine* engine)
    :  AtomicModel(engine)
{
    this->AddState("RESTART");
    this->AddState("RUNNING");
    this->AddState("WAIT");

    this->SetCurState("WAIT");
    
    this->AddInputPort("Start");
    // this->AddInputPort("Result");
    this->AddOutputPort("Restart");

    this->UpdateTime(0.0f);
}

bool Transducer::ExtTransFn(const std::string& inPort, const std::any& anyMessage) {
    if(inPort == "Start" && this->GetCurState() == "WAIT"){
        this->SetCurState("RUNNING");
    }else if(inPort == "Result" && this->GetCurState() == "RUNNING"){
        ResultMsg msg;
        if(TryCastMessage<ResultMsg>(anyMessage, msg, "Transducer::ExtTransFn")) return false;
        this->result_ = *(msg.res);
        
    }
    return true;
}
bool Transducer::OutputFn() {
    if (this->GetCurState() == "RESTART") {
        RestartMsg message;
        message.needChangeScenario = false;
        message.needChangeSeed = false;
        std::any anyMessage = message;
        this->AddOutputEvent("Restart",anyMessage);
    }else if (this->GetCurState() == "RUNNING") {
        // no output message, 

    }
    return true;
}
bool Transducer::IntTransFn() {
    if(this->GetCurState()=="RESTART"){
        this->SetCurState("RUNNING");
    }else if(this->GetCurState()=="RUNNING"){
        this->SetCurState("WAIT");
    }
    return true;
}

float Transducer::TimeAdvanceFn() {
    if (this->GetCurState() == "RESTART") return 0.0f;
    if (this->GetCurState() == "RUNNING") return simulationEndTime_;
    if(this->GetCurState()=="WAIT") return TIME_INF;
    return -1;
}

// column headers: seed, blueCasualties, redCasualties, totalScore
bool Transducer::StoreResultToCSV(const std::string& path, const Result& result) {
    std::ofstream ofs(path, std::ios::app);
    if (!ofs.is_open()) {
        LogError(this->engine->GetCurrentTime(), this->GetName(), "Failed to open result file:", path);
        return false;
    }

    ofs << result.seed << ","
        << result.blueCasualties << ","
        << result.redCasualties << ","
        << result.totalScore << "\n";

    ofs.close();
    return true;
}