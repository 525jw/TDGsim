#pragma once
#include "model.h"
#include "event.h"
#include <any>
#include <string>

#define TIME_TYPE float

class AtomicModel : public Model{
private:

public:
    virtual bool HandleExtEvent(inPort,) {return false;}
    virtual bool ExtTransFn(const std::any& message) {return false;}
    virtual bool IntTransFn() {return false;}
    virtual bool OuputFn() {return false;}
    virtual TIME_TYPE TimeAdvanceFn() {return -1;}
};