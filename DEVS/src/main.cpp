#pragma once
#include "engine.hpp"
#include "sim_world.hpp"
#include <iostream>

int main(){
    SimWorld simWorld = SimWorld();
    Engine engine = Engine();
    engine.SetOutMostModel(simWorld);
    engine.Run();
    return 0;
}