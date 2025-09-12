#include "generator.hpp"
#include <ctime>

Generator::Generator(Engine* engine)
    :  AtomicModel(engine)
{
    this->AddState("START");
    this->AddState("WAIT");

    this->SetCurState("START");
    
    this->AddInputPort("Restart");
    this->AddOutputPort("Start");

    this->seed = GenerateSeed();
}

unsigned int Generator::GenerateSeed() {
    return static_cast<unsigned int>(time(nullptr));
}

bool Generator::ExtTransFn(const std::string& inPort, const std::any& anyMessage) {
    if(inPort == "Restart" && this->GetCurState() == "WAIT"){
        // RestartMsg message;
        // if(!TryCastMessage(anyMessage, message, "Generator::ExtTransFn::Restart")) return false;
        // this->SetCurState("START");
    }
    return true;
}

bool Generator::IntTransFn() {
    if(this->GetCurState()=="START"){
        this->SetCurState("WAIT");
    }
    return true;
}

bool Generator::OutputFn() {
    if (this->GetCurState() == "START") {
        Start message;
        message.seed = this->seed;     
        std::any anyMessage = message;
        this->AddOutputEvent("Start",anyMessage);
    }
    return true;
}
TIME_T Generator::TimeAdvanceFn() {
    if (this->GetCurState() == "START") {
        return 0.0f;
    }else if(this->GetCurState()=="WAIT"){
        return TIME_INF;
    }
    return -1;
}