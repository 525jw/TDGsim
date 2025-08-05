#include "decision_maker.hpp"

DecisionMaker::DecisionMaker(Engine* engine, int objectID)
    : AtomicModel(engine)
{
    this->objectID = objectID;

    this->AddState("WAIT");
    this->AddState("APPROACH");
    this->AddState("FIRE");
    this->AddState("DECIDE");

    this->SetCurState("WAIT");

    this->AddInputPort ("mnvRes");
    this->AddOutputPort("detRes");
    this->AddInputPort ("deadInfo");
}

bool DecisionMaker::ExtTransFn(const std::string& inPort, const std::any& anyMessage){
    if(this->GetCurState()=="APPROACH"){
        if(inPort=="mnvRes"){
            MnvRes message;
            if(!TryCastMessage(anyMessage, message, "")) return false;
            this->curPos= message.curPos;
            if(this->detPos == this->curPos) // 천이조건검사
                this->SetCurState("DECIDE");

        }else if(inPort=="detRes"){
            DetRes message;
            if(!TryCastMessage(anyMessage,message,"")) return false;
            if(message.enemyDetected){ // 천이조건검사
                this->enemyPos=message.enemyPos;
                this->SetCurState("DECIDE");
            }
        }else if(inPort=="deadInfo"){
            DeadInfo message;
            if(!TryCastMessage(anyMessage,message,"")) return false;
            this->SetCurState("WAIT");
        }
    }
    if (inPort=="mnvRes" && this->GetCurState()=="APPROACH") {
        EnvInfo message;
        if(!TryCastMessage(anyMessage,message,"")) return false;

        this->RebuildEnemyDistance(message);
    } else if (inPort=="mnvRes") {
        MnvRes message;
        if(!TryCastMessage(anyMessage,message,"")) return false;

        this->curPos=message.curPos;
    } else if (inPort=="deadInfo") {
        this->SetCurState("WAIT");
        enemyPos.clear();
        return true;
    }
    if (!enemyPos.empty()) this->SetCurState("DETECT");
    else this->SetCurState("WAIT");
    return true;
}


bool DecisionMaker::IntTransFn(){
    if (this->GetCurState() == "APPROACH") {

    }else if(this->GetCurState()=="FIRE"){

    }
    return true;
}

bool DecisionMaker::OutputFn(){
    if(this->GetCurState()=="DETECT"){
        DetRes message;
        // if(EnemyIsVisible())
            message.enemyDetected = true;
        // else
            // message.enemyDetected = false;
        message.enemyPos = this->enemyPos;
        std::any anyMessage = message;
        this->AddOutputEvent("detRes",anyMessage);
    }
    return true;
}

TIME_T DecisionMaker::TimeAdvanceFn(){
    if (this->GetCurState() == "WAIT")   return TIME_INF;
    if (this->GetCurState() == "DETECT") return t_dc;
    return -1;   // 오류
}