#include "environment.hpp"

Environment::Environment(Engine* engine)
    : AtomicModel(engine)
{
    this->AddState("IDLE");
    this->AddState("WAIT");
    this->AddState("UPDATE");

    this->SetCurState("IDLE");

    this->AddInputPort("scenInfo");
    this->AddInputPort("mnvRes");
    this->AddInputPort("deadInfo");

    this->AddOutputPort("envInfo");
}

bool Environment::ExtTransFn(const std::string& inPort, const std::any& anyMessage) {
    if(inPort == "mnvRes" && this->GetCurState() == "WAIT"){
        MnvRes message;
        if(!TryCastMessage(anyMessage,message,"")) return false;

        switch (message.team){
        case Team::RED:
            redTeam[message.objectID]=message.curPos;
            break;
        case Team::BLUE:
            blueTeam[message.objectID]=message.curPos;
            break;
        default:
            std::cerr << "[ERROR] Unknown team in MnvRes message.\n";
            return false;
        }

        this->SetCurState("UPDATE");
    }else if(inPort == "deadInfo" && this->GetCurState()=="WAIT"){
        DeadInfo message;
        if(!TryCastMessage(anyMessage,message,"")) return false;
        switch (message.team){
        case Team::RED:
            redTeam.erase(message.objectID);
            break;
        case Team::BLUE:
            blueTeam.erase(message.objectID);
            break;
        default:
            std::cerr << "[ERROR] Unknown team in DeadInfo message.\n";
            return false;
        }

        this->SetCurState("UPDATE");
    }else if(inPort == "scenInfo" && this->GetCurState()=="IDLE"){
        ScenInfo message;
        if(!TryCastMessage(anyMessage,message,"")) return false;
        this->redTeam=message.redTeam;
        this->blueTeam=message.blueTeam;
        
        this->SetCurState("WAIT");
    }
    return true;
}

bool Environment::IntTransFn() {
    if(this->GetCurState()=="UPDATE"){
        this->SetCurState("WAIT");
    }
    return true;
}

bool Environment::OutputFn() {
    if(this->GetCurState()=="UPDATE"){
        EnvInfo message;
        message.blueTeam = this->blueTeam;
        message.redTeam = this->redTeam;
        std::any anyMessage = message;
        this->AddOutputEvent("envInfo",anyMessage);
    }
    return true;
}

TIME_T Environment::TimeAdvanceFn() {
    if (this->GetCurState() == "WAIT") return TIME_INF;
    if (this->GetCurState() == "UPDATE") return 0.0f;
    return -1;
}