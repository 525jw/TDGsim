#pragma once
#include "DEVS/coupled_model.hpp"
#include "maneuver.hpp"
#include "fire.hpp"
#include "detection.hpp"
#include "DEVS/logger.hpp"

class Engine;

class BlueSoldier : public CoupledModel {
private:
    int entityId;
    std::string name;
public:
    BlueSoldier(Engine* engine, Point spawnPos, std::string name)
        : CoupledModel(engine)
    {
        this->name = name;

        if (!EnvGlobal::Ready()) {
            throw std::runtime_error("Environment not ready (EnvGlobal::ptr is null)");
        }
        entityId = ENV_REF.RegisterEntity(spawn, Team::BLUE, ForceType::BLUE_INF, name);
        if (entityId < 0) {
            throw std::runtime_error("Failed to register entity in Environment");
        }
        this->entityId = 
        Maneuver* mnv = new Maneuver(engine);
        mnv->SetParentModel(this);
    }
};
