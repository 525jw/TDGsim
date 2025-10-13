#include "DEVS/engine.hpp"
#include "tdg_sim.hpp"

int main(){
    std::puts("TDG starts");

    Engine engine;

    TdgSim tdgSim(&engine);
    engine.SetRootModel(&tdgSim);
    engine.Run();
    
    std::puts("TDG done");
    return 0;
}