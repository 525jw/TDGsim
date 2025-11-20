#include "hq.hpp"
#include "order_reader.hpp"
// ------------- HQ class -------------
HQ::HQ(Engine* engine,
       std::vector<int>* memberIds,
       SideType side,
       std::string ordersFile)
    : AtomicModel(engine),
      hqSide(side),
      ordersFilePath(std::move(ordersFile)) {
    this->engine = engine;

    if (memberIds) {
        this->memberIds.insert(memberIds->begin(), memberIds->end());
    }

    this->AddState("WAIT");
    this->AddState("DECIDE");
    this->AddState("REPORT");

    this->SetCurState("WAIT");

    AddInputPort("Start");
    AddOutputPort("CompanyOrd");
    AddInputPort("InfantryRep");

}

bool HQ::ExtTransFn(const std::string& inPort, const std::any& anyMessage) {
    if (inPort == "Start"){
        // Indirect message — payload contained in json file
        this->SetCurState("DECIDE");
    }else if(inPort == "InfantryRep"){
        this->SetCurState("REPORT");
    }
    return true;
}
bool HQ::OutputFn() {
    if (this->GetCurState() == "DECIDE") {
        CompanyOrd order = LoadOrderFromFile(
            ordersFilePath,
            hqSide,
            memberIds);

        LogSimulation(this->engine->GetCurrentTime(),this->GetNameWithId(),"LOAD_ORDER");
        if (order.orders.empty()) {
            LogSimulation(this->engine->GetCurrentTime(),this->GetNameWithId(),"LOAD_ORDER"," orders are empty");
        } else {
            for (const auto& [entityId, ordList] : order.orders) {
                for (std::size_t idx = 0; idx < ordList.size(); ++idx) {
                    const Order& ord = ordList[idx];
                    std::string taskStr;
                    switch (ord.task) {
                        case TaskType::MOVE: taskStr = "MOVE"; break;
                        case TaskType::BOMBARD: taskStr = "BOMBARD"; break;
                        case TaskType::HOLD: taskStr = "HOLD"; break;
                        default: taskStr = "UNKNOWN"; break;
                    }
                    if (ord.hasDestination) {
                        LogSimulation(this->engine->GetCurrentTime(),this->GetNameWithId(),"LOAD_ORDER"," entityId=",entityId," idx=",idx," task=",taskStr," to=",ord.to.x,",",ord.to.y);
                    } else {
                        LogSimulation(this->engine->GetCurrentTime(),this->GetNameWithId(),"LOAD_ORDER"," entityId=",entityId," idx=",idx," task=",taskStr," without destination");
                    }
                }
            }
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
