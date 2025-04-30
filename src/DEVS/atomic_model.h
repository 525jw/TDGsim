#include "model.h"

#define TIME_TYPE float

class AtomicModel : public Model{
private:

public:
    virtual bool ExtTransFn(const message &) {return false;}
    virtual bool IntTransFn() {return false;}
    virtual bool OuputFn() {return false;}
    virtual TIME_TYPE TimeAdvanceFn() {return -1;}
};