#pragma once
#include "engine.hpp"
#include "test_cannonsys.cpp"
#include <iostream>
#include <vector>

std::vector<std::vector<int>> worldMap(5,std::vector<int>(5,0));

int main(){
    Engine engine = Engine();
    TestCannonSys testCannonsys = TestCannonSys(&engine);
    engine.BuildDEVS(&testCannonsys);
    engine.Run();
    return 0;
}