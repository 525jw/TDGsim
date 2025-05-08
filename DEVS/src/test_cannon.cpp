// #pragma once
// #include "atomic_model.hpp"
// #include <any>

// class TestCannon : public AtomicModel{
// private:
//     TIME_T time;
//     TIME_T nextTime;
//     Engine* engine;
//     std::any info;
// public:    
//     bool ExtTransFn(const std::string inPort, const std::any& message) {
//         if(inPort == "damageinfo"){
//             if(message->damage > 0.5){
//                 this->SetCurState("DEAD");
//             }
//         }
//         return true;
//     }

//     bool IntTransFn() {
//         if(this->GetCurState() == "ENGAGE"){
//             if(this->time - this->info->engageStartTime > 2.0){
//                 this->SetCurState("Ready");
//             }
//         }
//         else if(this->GetCurState() == "READY"){
//             if(this->time - this->info->readyStartTime > 1.0){
//                 this->SetCurState("Engage");
//             }
//         }
//         return true;
//     }

//     bool OutputFn() {return false;}

//     TIME_T TimeAdvanceFn() {
//         if 
//         return ;
//     }
// };