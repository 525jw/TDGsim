#pragma once
#include "DEVS/atomic_model.hpp"
#include "message.hpp"
#include "DEVS/logger.hpp"
#include "SIM/environment.hpp"
#include <random>

class RedSoldier : public AtomicModel{
private:
    int entityId;
    std::string name;

    Point curPos={-1, -1};
    std::vector<int> enemyIds;   // key: Enemy entityId, value: enemy pos (x,y)
    int vision = 10;
    Team team = Team::RED;

    bool enemyDetected = false;

    // TIME_T fireFreq = 0.01f;
    // int ammo = 30;
    TIME_T reloadTime = 1.0f;
    float accuracy = 0.7f;
    int targetId = -1;

    // RNG
    std::mt19937 rng;
    bool rngInit = false;

    TIME_T t_fire = -1.0f;
    TIME_T fireEquation();
public:
    RedSoldier(Engine* engine, int entityId, std::string name);

    bool ExtTransFn(const std::string& inPort, const std::any& anyMessage);
    bool IntTransFn();
    bool OutputFn();
    TIME_T TimeAdvanceFn();
    
    void RebuildEnemyPosList();
};