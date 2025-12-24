#include "hq.hpp"
#include "tdg_sim/data_loader.hpp"
// ------------- HQ class -------------
HQ::HQ(Engine* engine,
       std::vector<int>* memberIds,
       SideType side,
       std::string_view bmlPath)
    : AtomicModel(engine),
      side_(side),
      bmlPath_(bmlPath){
    this->engine = engine;

    if (memberIds) {
        this->memberIds_.insert(memberIds->begin(), memberIds->end());
    }

    this->AddState("WAIT");
    this->AddState("DECIDE");
    this->AddState("REPORT");

    this->SetCurState("WAIT");

    AddInputPort("Start");
    AddOutputPort("CompanyOrd");
    AddInputPort("InfantryRep");

    this->UpdateTime(0.0f);
}

bool HQ::ExtTransFn(const std::string& inPort, const std::any& anyMessage) {
    if (inPort == "Start"){
        this->SetCurState("DECIDE");
    }else if(inPort == "InfantryRep"){
        this->SetCurState("REPORT");
    }
    return true;
}
bool HQ::OutputFn() {
    if (this->GetCurState() == "DECIDE") {
        CompanyOrd order;
        const bool loaded = data_loader::LoadOrderFromFile(this->bmlPath_,this->side_,order);
        if (!loaded || order.orders.empty()) {
            LogSimulation(this->engine->GetCurrentTime(),this->GetNameWithId(),"LOAD_ORDER"," failed to load file");
        }else{
            LogSimulation(this->engine->GetCurrentTime(),this->GetNameWithId(),"LOAD_ORDER");
        }
        std::any anyOrder = order;
        this->AddOutputEvent("CompanyOrd", anyOrder);
    }
    return true;
}
bool HQ::IntTransFn() {
    if(this->GetCurState()=="DECIDE"){
        this->SetCurState("WAIT");
    }
    return true;
}

float HQ::TimeAdvanceFn() {
    if(this->GetCurState()=="WAIT") return TIME_INF;
    if(this->GetCurState()=="DECIDE") return t_dec;
    if(this->GetCurState()=="REPORT") return t_rep;
    return -1;
}
