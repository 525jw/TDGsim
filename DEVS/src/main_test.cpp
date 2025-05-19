#pragma once
#include "engine.hpp"
#include "test_cannonsys.cpp"
#include "test_world.hpp"
#include "logger.hpp"
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>


int main(){
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    Engine engine = Engine();
    TestCannonSys testCannonsys = TestCannonSys(0,&engine);
    engine.BuildDEVS(&testCannonsys);
    logger<<"sim starts"<<std::endl;
    engine.Run();
    return 0;
}
/*
note: test codes에서 id는 하드코딩 되어있음
system 0
blue team 1
red team 2
blue team first cannon 11
blue team second cannon 12
blue team third cannon 13
red team first cannon 21
red team second cannon 22
red team thid cannon 23
TODO : model id 자동화
*/