#pragma once
#include "engine.hpp"
#include "test_cannonsys.cpp"
#include "test_world.hpp"
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime> 


int main(){
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    Engine engine = Engine();
    TestCannonSys testCannonsys = TestCannonSys(&engine);
    engine.BuildDEVS(&testCannonsys);
    engine.Run();
    return 0;
}