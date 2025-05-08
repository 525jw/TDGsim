#pragma once
#include "engine.hpp"
#include "test_cannonsys.cpp"
#include <iostream>

int main(){
    TestCannonSys testCannonsys = TestCannonSys();
    Engine engine = Engine();
    engine.SetOutMostModel(testCannonSys);
    engine.Run();
    return 0;
}