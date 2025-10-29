#include "DEVS/engine.hpp"
#include "tdg_sim.hpp"

int main(){
    std::puts("TDG starts");

    Engine engine;

    TdgSim tdgSim(&engine);
    engine.SetRootModel(&tdgSim);
    engine.Run();

    // 시뮬레이션 결과 집계
    if (EnvReady()) {
        const unsigned int seed = env->GetSeed();

        // 1) 목표 구역 내 BLUE/RED 인원 수 (x:83~87, y:29~33)
        // map 수정 시 목표 구역 수정 필요
        const int x1 = 83, x2 = 87, y1 = 29, y2 = 33;
        int blueInObj = 0, redInObj = 0;

        const int W = env->GetWidth();
        const int H = env->GetHeight();
        const int X1 = std::max(0, std::min(x1, W - 1));
        const int X2 = std::max(0, std::min(x2, W - 1));
        const int Y1 = std::max(0, std::min(y1, H - 1));
        const int Y2 = std::max(0, std::min(y2, H - 1));

        // 목표구역 스캔
        for (int y = Y1; y <= Y2; ++y) {
            for (int x = X1; x <= X2; ++x) {
                for (int id : env->QueryEntityIdsAt(Point{x, y})) {
                    if (const Entity* e = env->QueryEntityById(id)) {
                        if (e->side == Side::BLUE) ++blueInObj;
                        if (e->side == Side::RED) ++redInObj;
                    }
                }
            }
        }

        // 2) 전체 생존자/사상자
        std::unordered_set<int> aliveIds;
        int aliveBlue = 0, aliveRed = 0;

        for (int y = 0; y < H; ++y) {
            for (int x = 0; x < W; ++x) {
                auto ids = env->QueryEntityIdsAt(Point{x, y});
                for (int id : ids) {
                    if (aliveIds.insert(id).second) {
                        if (const Entity* e = env->QueryEntityById(id)) {
                            if (e->side == Side::BLUE) ++aliveBlue;
                            else if (e->side == Side::RED) ++aliveRed;
                        }
                    }
                }
            }
        }

        const int initBlue = env->GetInitialBlue();
        const int initRed  = env->GetInitialRed();
        const int kiasBlue = std::max(0, initBlue - aliveBlue);
        const int kiasRed  = std::max(0, initRed  - aliveRed);

        // 콘솔 출력
        std::printf("\n=== Simulation Result ===\n");
        std::printf("Seed: %d\n", seed);
        std::printf("Objective Area (x:%d~%d, y:%d~%d): BLUE inside = %d RED inside = %d\n",
                    X1, X2, Y1, Y2, blueInObj, redInObj);
        std::printf("Alive   - BLUE: %d / RED: %d\n", aliveBlue, aliveRed);
        std::printf("Initial - BLUE: %d / RED: %d\n", initBlue, initRed);
        std::printf("Casualties (KIA/MIA approximated) - BLUE: %d / RED: %d\n", kiasBlue, kiasRed);
        std::printf("==========================\n\n");

        // 파일 출력 (result_summary.txt)
        std::FILE* fp = std::fopen("result_summary.txt", "w");
        if (fp) {
            std::fprintf(fp, "=== Simulation Result ===\n");
            std::fprintf(fp, "Seed: %d\n", seed);
            std::fprintf(fp, "Objective Area (x:%d~%d, y:%d~%d): BLUE inside = %d RED inside = %d\n",
                         X1, X2, Y1, Y2, blueInObj, redInObj);
            std::fprintf(fp, "Alive   - BLUE: %d / RED: %d\n", aliveBlue, aliveRed);
            std::fprintf(fp, "Initial - BLUE: %d / RED: %d\n", initBlue, initRed);
            std::fprintf(fp, "Casualties (KIA/MIA approximated) - BLUE: %d / RED: %d\n",
                         kiasBlue, kiasRed);
            std::fprintf(fp, "==========================\n");
            std::fclose(fp);
            std::printf("[OK] Saved summary to result_summary.txt\n");
        } else {
            std::fprintf(stderr, "[ERROR] Could not open result_summary.txt for writing.\n");
        }
    } else {
        std::fprintf(stderr, "[WARN] Environment is not ready. No summary produced.\n");
    }

    std::puts("TDG ends");
    return 0;
}
