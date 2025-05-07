#pragma once
#include "engine.hpp"
#include "test_world.hpp"
#include <iostream>

int main(){
    TestWorld testWorld = TestWorld();
    Engine engine = Engine();
    engine.SetOutMostModel(testWorld);
    engine.Run();
    return 0;
}