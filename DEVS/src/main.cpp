#pragma once
#include "engine.h"
#include "sim_world.h"
#include <iostream>

int main(){
    SimWorld simWorld = SimWorld();
    Engine engine = Engine();
    engine.SetOutMostModel(simWorld);
    engine.Run();
    return 0;
}