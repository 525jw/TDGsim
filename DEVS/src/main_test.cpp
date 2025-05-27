#include "engine.hpp"
#include "test_cansys.hpp"
#include "test_world.hpp"
#include "logger.hpp"
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iomanip>


int main() {
    std::cerr << "[DEBUG] Entered main()" << std::endl;

    Engine engine;
    std::cerr << "[DEBUG] Engine creation completed" << std::endl;

    TestCannonSys sys(0, &engine);
    std::cerr << "[DEBUG] TestCannonSys creation completed" << std::endl;

    engine.BuildDEVS(&sys);
    std::cerr << "[DEBUG] BuildDEVS completed" << std::endl;

    engine.Run();
    std::cerr << "[DEBUG] Run() finished" << std::endl;


    logger_world << "===== World Map =====" << std::endl;
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            logger_world << std::setw(2) << worldMap[i][j] << " ";
        }
        logger_world << std::endl;
    }

    logger_world << "\n===== Hit Count =====" << std::endl;
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            hitCountMap[i][j]/=3;
        }
    }
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            logger_world << std::setw(2) << hitCountMap[i][j] << " ";
        }
        logger_world << std::endl;
    }

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