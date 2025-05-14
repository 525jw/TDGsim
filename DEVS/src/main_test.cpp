#pragma once
#include "engine.hpp"
#include "test_cannonsys.cpp"
#include <iostream>

int main(){
    Engine engine = Engine();
    TestCannonSys testCannonsys = TestCannonSys(&engine);
    engine.SetOutMostModel(testCannonSys);
    engine.Run();
    return 0;
}