#include "generator.hpp"
#include "tdg_sim/data_loader.hpp"
#include <fstream>
#include <string>

Generator::Generator(Engine* engine)
    :  AtomicModel(engine)
{
    this->AddState("GENERATE");
    this->AddState("WAIT");

    this->SetCurState("GENERATE");
    
    this->AddInputPort("Restart");
    this->AddOutputPort("Start");

    seed_ = GenerateRandomSeed();
    this->experimentIdx_ = this->GetExperiementIndexFromCsv();
    int runIndex = this->experimentIdx_ + 1;
    ConfigureLogFiles(runIndex);
    this->UpdateTime(0.0f);
}
int Generator::GetExperiementIndexFromCsv() {
    const std::string filename(path::RESULT_CSV);
    int count = 0;
    std::ifstream ifs(filename);
    if (ifs.is_open()) {
        std::string line;
        bool firstLine = true;
        while (std::getline(ifs, line)) {
            if (line.empty()) continue;
            if (firstLine) {
                firstLine = false;
                continue;
            }
            ++count;
        }
    }
    return count;
}

unsigned int Generator::GenerateRandomSeed() {
    auto now = std::chrono::high_resolution_clock::now();
    return static_cast<unsigned int>(
        now.time_since_epoch().count()
    );
}

bool Generator::ExtTransFn(const std::string& inPort, const std::any& anyMessage) {
    if(inPort == "Restart" && this->GetCurState() == "WAIT"){
        RestartMsg msg;
        if (!TryCastMessage(anyMessage, msg, "Generator::ExtTransFn.Restart")) return false;
        this->SetCurState("GENERATE");
    }
    return true;
}
bool Generator::OutputFn() {
    if (this->GetCurState() == "GENERATE") {
        if(!data_loader::LoadScenarioFromFile(path::SCENARIO_JSON, scenario_)){
            // error
        }
        this->experimentIdx_++;
        if(scenario_.seed == 0 ) scenario_.seed = seed_;
        
        LogSimulation(this->GetNextTime(),this->GetName(),"SIMULATION GENERATION","seed=",scenario_.seed,
                      " experiment count=",this->experimentIdx_);
        StartMsg message;
        message.scen = &scenario_;     
        message.experimentIndex = this->experimentIdx_;
        std::any anyMessage = message;
        this->AddOutputEvent("Start",anyMessage);
    }
    return true;
}
bool Generator::IntTransFn() {
    if(this->GetCurState()=="GENERATE"){
        this->SetCurState("WAIT");
    }
    return true;
}

float Generator::TimeAdvanceFn() {
    if (this->GetCurState() == "GENERATE") return 0.0f;
    if(this->GetCurState()=="WAIT") return TIME_INF;
    return -1;
}
