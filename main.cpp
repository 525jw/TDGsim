#include "DEVS/engine.hpp"
#include "tdg_sim.hpp"
#include <cstdlib>

int main(){
    std::puts("TDG starts");

    Engine engine;

    TdgSim tdgSim(&engine);
    engine.SetRootModel(&tdgSim);

    engine.Run();

    int SimCnt = 1; // 시뮬레이션 실행 횟수

    // RSH
    // 시뮬레이션 결과 집계
    {
        if (EnvReady()) {
            const unsigned int seed = env->GetSeed();

            // [고정 좌표] 목표 구역 (area1~3)
            const int A1_x1_disp = 21, A1_x2_disp = 37, A1_y1_disp = 84, A1_y2_disp = 100;
            const int A2_x1_disp = 63, A2_x2_disp = 79, A2_y1_disp = 84, A2_y2_disp = 100;
            const int A3_x1_disp = 53, A3_x2_disp = 80, A3_y1_disp = 15, A3_y2_disp = 25;
            
            const int W = env->GetWidth();
            const int H = env->GetHeight();
            
            // 사각형 내 BLUE/RED 카운트 함수
            auto countInRect = [&](int x1, int x2, int y1, int y2, int& blueCnt, int& redCnt) {
                // 좌표 정렬
                if (x1 > x2) std::swap(x1, x2);
                if (y1 > y2) std::swap(y1, y2);
                // 맵 경계 보정
                const int X1 = std::max(0, std::min(x1, W - 1));
                const int X2 = std::max(0, std::min(x2, W - 1));
                const int Y1 = std::max(0, std::min(y1, H - 1));
                const int Y2 = std::max(0, std::min(y2, H - 1));
                
                // 1) 목표 구역 내 생존자 카운트 
                blueCnt = 0; redCnt = 0;
                for (int y = Y1; y <= Y2; ++y) {
                    for (int x = X1; x <= X2; ++x) {
                        for (int id : env->QueryEntityIdsAt(Point{x, y})) {
                            if (const Entity* e = env->QueryEntityById(id)) {
                                if (e->side == Side::BLUE) ++blueCnt;
                                else if (e->side == Side::RED) ++redCnt;
                            }
                        }
                    }
                }
            };

            // ── 카운트 ──
            int blueInA1 = 0, redInA1 = 0;
            int blueInA2 = 0, redInA2 = 0;
            int blueInA3 = 0, redInA3 = 0;

            countInRect(A1_x1_disp, A1_x2_disp, A1_y1_disp, A1_y2_disp, blueInA1, redInA1);
            countInRect(A2_x1_disp, A2_x2_disp, A2_y1_disp, A2_y2_disp, blueInA2, redInA2);
            countInRect(A3_x1_disp, A3_x2_disp, A3_y1_disp, A3_y2_disp, blueInA3, redInA3);

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
            const int kiasBlue = std::max(0, initBlue - aliveBlue -1);  // -1: 고정
            const int kiasRed  = std::max(0, initRed  - aliveRed);

            // 목표 구역 가중치
            const double wA1 = 0.2;
            const double wA2 = 0.3;
            const double wA3 = 0.5;

            // 3) 점수 계산
            auto areaScore = [](int blueCnt, int redCnt, double w) -> double {
                double factor = 0.0;
                if (blueCnt > 0 && redCnt > 0) {
                    factor = 0.5; // 적군, 아군 
                } else if (blueCnt > 0 && redCnt == 0) {
                    factor = 1.0; // 아군만
                } else {
                    factor = 0.0; // 적군만 또는 없음
                }
                return w * factor;
            };

            const double scoreA1 = areaScore(blueInA1, redInA1, wA1);
            const double scoreA2 = areaScore(blueInA2, redInA2, wA2);
            const double scoreA3 = areaScore(blueInA3, redInA3, wA3);
            const double totalScore = scoreA1 + scoreA2 + scoreA3;

            // 환경변수 SIMCNT가 있으면 사용 (배치 실행 시 외부에서 주입)
            if (const char* envCnt = std::getenv("SIMCNT")) {
                int v = std::atoi(envCnt);
                if (v > 0) SimCnt = v;
            }
            // 콘솔 출력
            std::printf("\n=== Simulation Result %d ===\n", SimCnt);
            std::printf("Seed: %d\n", seed);
            std::printf("Objective Area 1 (x:%d~%d, y:%d~%d): BLUE inside = %d RED inside = %d\n",
                        A1_x1_disp, A1_x2_disp, A1_y1_disp, A1_y2_disp, blueInA1, redInA1);
            std::printf("Objective Area 2 (x:%d~%d, y:%d~%d): BLUE inside = %d RED inside = %d\n",
                        A2_x1_disp, A2_x2_disp, A2_y1_disp, A2_y2_disp, blueInA2, redInA2);
            std::printf("Objective Area 3 (x:%d~%d, y:%d~%d): BLUE inside = %d RED inside = %d\n",
                        A3_x1_disp, A3_x2_disp, A3_y1_disp, A3_y2_disp, blueInA3, redInA3);
            std::printf("Alive   - BLUE: %d / RED: %d\n", aliveBlue, aliveRed);
            std::printf("Initial - BLUE: %d / RED: %d\n", initBlue, initRed);
            std::printf("Casualties - BLUE: %d / RED: %d\n", kiasBlue, kiasRed);
            std::printf("Score Weights: Area1=%.1f Area2=%.1f Area3=%.1f\n", wA1, wA2, wA3);
            std::printf("Area1 Score: %.3f (blue=%d, red=%d)\n", scoreA1, blueInA1, redInA1);
            std::printf("Area2 Score: %.3f (blue=%d, red=%d)\n", scoreA2, blueInA2, redInA2);
            std::printf("Area3 Score: %.3f (blue=%d, red=%d)\n", scoreA3, blueInA3, redInA3);
            std::printf("Total Score: %.3f\n", totalScore);
            std::printf("==========================\n\n");

            // 파일 출력 (result_summary.txt)
            std::FILE* fp = std::fopen("result_summary.txt", "a");
            if (fp) {
                // 기존 내용이 있으면 구분을 위해 한 줄 공백 추가
                std::fseek(fp, 0, SEEK_END);
                long fsize = std::ftell(fp);
                if (fsize > 0) {
                    std::fprintf(fp, "\n");
                }
                std::fprintf(fp, "=== Simulation Result %d ===\n", SimCnt);
                std::fprintf(fp, "Seed: %d\n", seed);
                std::fprintf(fp, "Objective Area 1 (x:%d~%d, y:%d~%d): BLUE inside = %d RED inside = %d\n",
                            A1_x1_disp, A1_x2_disp, A1_y1_disp, A1_y2_disp, blueInA1, redInA1);
                std::fprintf(fp, "Objective Area 2 (x:%d~%d, y:%d~%d): BLUE inside = %d RED inside = %d\n",
                            A2_x1_disp, A2_x2_disp, A2_y1_disp, A2_y2_disp, blueInA2, redInA2);
                std::fprintf(fp, "Objective Area 3 (x:%d~%d, y:%d~%d): BLUE inside = %d RED inside = %d\n",
                            A3_x1_disp, A3_x2_disp, A3_y1_disp, A3_y2_disp, blueInA3, redInA3);
                std::fprintf(fp, "Alive   - BLUE: %d / RED: %d\n", aliveBlue, aliveRed);
                std::fprintf(fp, "Initial - BLUE: %d / RED: %d\n", initBlue, initRed);
                std::fprintf(fp, "Casualties - BLUE: %d / RED: %d\n", kiasBlue, kiasRed);
                std::fprintf(fp, "Score Weights: Area1=%.1f Area2=%.1f Area3=%.1f\n", wA1, wA2, wA3);
                std::fprintf(fp, "Area1 Score: %.3f (blue=%d, red=%d)\n", scoreA1, blueInA1, redInA1);
                std::fprintf(fp, "Area2 Score: %.3f (blue=%d, red=%d)\n", scoreA2, blueInA2, redInA2);
                std::fprintf(fp, "Area3 Score: %.3f (blue=%d, red=%d)\n", scoreA3, blueInA3, redInA3);
                std::fprintf(fp, "Total Score: %.3f\n", totalScore);
                std::fprintf(fp, "==========================\n");
                std::fclose(fp);
                std::printf("[OK] Saved summary to result_summary.txt\n");
            } else {
                std::fprintf(stderr, "[ERROR] Could not open result_summary.txt for appending.\n");
            }
        } else {
            std::fprintf(stderr, "[WARN] Environment is not ready. No summary produced.\n");
        }
    }
    
    std::puts("TDG ends");
    return 0;
}
