#include "blue_hq.hpp"

BlueHQ::BlueHQ(Engine* engine)
    : AtomicModel(engine)
{
    this->engine=engine;

    this->AddState("WAIT");
    this->AddState("DECIDE");

    this->SetCurState("WAIT");

    AddInputPort("Start");
    AddOutputPort("CompanyOrd");
    AddInputPort("InfantryRep");
}

bool BlueHQ::ExtTransFn(const std::string& inPort, const std::any& anyMessage) {
    if (inPort == "Start"){
        this->SetCurState("DECIDE");
    }else if(inPort == "InfantryRep"){
        this->SetCurState("DECIDE");
    }
    return true;
}
bool BlueHQ::OutputFn() {
    if (this->GetCurState()=="DECIDE"){
        CompanyOrd order;
        // 명령
        std::any anyOrder = order;
        this->AddOutputEvent("CompanyOrd",anyOrder);
    }
    return true;
}
bool BlueHQ::IntTransFn() {
    
    return true;
}

TIME_T BlueHQ::TimeAdvanceFn() {
    if(this->GetCurState()=="WAIT") return TIME_INF;
    if(this->GetCurState()=="DECIDE") return t_dec;
    return -1;
}