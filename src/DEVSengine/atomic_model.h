#include "basic_model.h"

#define TIME_TYPE float

class AtomicModel{
private:

public:
    virtual bool ExtTransFn(const message &) {return false;}
    virtual bool IntTransFn() {return false;}
    virtual bool OuputFn() {return false;}
    virtual TIME_TYPE TimeAdvanceFn() {return -1;}
};