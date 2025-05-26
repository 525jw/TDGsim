#include "engine.hpp"
#include "test_cansys.hpp"
#include "test_world.hpp"
#include "logger.hpp"
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>


int main() {
    std::cerr << "[DEBUG] main 진입" << std::endl;

    Engine engine;
    std::cerr << "[DEBUG] Engine 생성 완료" << std::endl;

    TestCannonSys sys(0, &engine);
    std::cerr << "[DEBUG] TestCannonSys 생성 완료" << std::endl;

    engine.BuildDEVS(&sys);
    std::cerr << "[DEBUG] BuildDEVS 완료" << std::endl;

    engine.Run();
    std::cerr << "[DEBUG] Run() 종료" << std::endl;

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