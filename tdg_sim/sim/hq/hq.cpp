#include "hq.hpp"
#include "tdg_sim/data_loader.hpp"

namespace {
void AppendOrders(CompanyOrd& dst, const CompanyOrd& src) {
    for (const auto& pair : src.orders) {
        const int entityId = pair.first;
        const auto& orders = pair.second;
        auto& out = dst.orders[entityId];
        out.insert(out.end(), orders.begin(), orders.end());
    }
}
}

// ------------- HQ class -------------
HQ::HQ(Engine* engine,
       std::vector<int>* memberIds,
       SideType side,
       std::string_view bmlPath,
       std::string_view npcPath)
    : AtomicModel(engine),
      side_(side),
      bmlPath_(bmlPath),
      npcPath_(npcPath){
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
    auto loadAndAppend = [&](std::string_view path, const char* sourceTag, bool& loadedFlag) {
        if (loadedFlag) {
            return;
        }
        CompanyOrd order;
        const bool loaded = data_loader::LoadOrderFromFile(path, this->side_, order);
        if (!loaded || order.orders.empty()) {
            LogSimulation(this->engine->GetCurrentTime(), this->GetNameWithId(),
                          "LOAD_ORDER", "source=", sourceTag, " failed to load file");
        } else {
            LogSimulation(this->engine->GetCurrentTime(), this->GetNameWithId(),
                          "LOAD_ORDER", "source=", sourceTag);
            AppendOrders(pendingOrders_, order);
        }
        loadedFlag = true;
    };

    if (inPort == "Start"){
        loadAndAppend(this->npcPath_, "npc", npcLoaded_);
        this->SetCurState("DECIDE");
    }else if(inPort == "InfantryRep"){
        loadAndAppend(this->bmlPath_, "bml", bmlLoaded_);
        this->SetCurState("DECIDE");
    }
    return true;
}

bool HQ::OutputFn() {
    if (this->GetCurState() == "DECIDE") {
        std::any anyMessage = pendingOrders_;
        pendingOrders_ = CompanyOrd{};
        this->AddOutputEvent("CompanyOrd", anyMessage);
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
