#pragma once
#include "model.h"
#include "event.h"
#include <any>
#include <string>

class AtomicModel : public Model{
private:
    time_t time=0;
public:
    AtomicModel(int modelID);

    // Ref: System Modeling Simulation, KTG, 108p 2-6-8
    virtual bool HandleExtEvent(const std::string inPort, time_t curTime) {return false;}
    virtual bool HandleTimeAdvance() {return false;}

    virtual bool ExtTransFn(const std::any& message) {return false;}
    virtual bool IntTransFn() {return false;}
    virtual bool OuputFn() {return false;}
    virtual time_t TimeAdvanceFn() {return -1;}

    const time_t getTime() const;
    void addOutputEvent();
};
