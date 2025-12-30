#pragma once
#include "DEVS/coupled_model.hpp"
#include "DEVS/logger.hpp"
#include "tdg_sim/common.hpp"
#include "tdg_sim/data_loader.hpp"
#include "tdg_sim/path.hpp"
#include "tdg_sim/sim/environment/environment.hpp"
#include "tdg_sim/sim/infantry/soldier/soldier.hpp"
#include "tdg_sim/sim/infantry/platoon_leader/platoon_leader.hpp"
#include "tdg_sim/sim/hq/hq.hpp"
#include "tdg_sim/sim/artillery/artillery.hpp"
class BlueForce : public CoupledModel{
public:
    BlueForce(Engine* engine)
    : CoupledModel(engine)
    {
        
        std::vector<int> cmp_Ids; cmp_Ids.clear();
        std::vector<std::string> platoon_names;
        std::vector<std::vector<Soldier*>> platoons;
        std::vector<std::vector<int>> platoon_ids;
        std::vector<PlatoonLeader*> platoon_leaders;

        Scenario scenario;
        const bool hasScenario = data_loader::LoadScenarioFromFile(path::SCENARIO_JSON, scenario);

        auto starts_with = [](const std::string& value, const std::string& prefix) {
            return value.rfind(prefix, 0) == 0;
        };
        auto platoon_key = [](const std::string& name) {
            const std::string token = "-SOL";
            const std::size_t pos = name.find(token);
            if (pos == std::string::npos) return name;
            return name.substr(0, pos);
        };
        auto get_platoon_index = [&](const std::string& name) {
            for (std::size_t i = 0; i < platoon_names.size(); ++i) {
                if (platoon_names[i] == name) return i;
            }
            platoon_names.push_back(name);
            platoons.emplace_back();
            platoon_ids.emplace_back();
            return platoon_names.size() - 1;
        };

        if (hasScenario) {
            for (const auto& entity : scenario.entities) {
                if (entity.side != SideType::BLUE) continue;
                if (entity.forceType != ForceType::RIFLE) continue;
                if (!starts_with(entity.name, "BLUE-PLT")) continue;

                const std::string key = platoon_key(entity.name);
                const std::size_t idx = get_platoon_index(key);
                const int id = env->RegisterEntityIdByName(entity.name);
                platoons[idx].push_back(
                    new Soldier(engine, Entity{id, entity.name, entity.side, entity.forceType, entity.position})
                );
                platoon_ids[idx].push_back(id);
            }
        }

        for (auto& platoon : platoons) {
            for (auto* soldier : platoon) {
                soldier->SetParentModel(this);
                this->RegisterSubModel(soldier);
            }
        }

        for (std::size_t i = 0; i < platoon_names.size(); ++i) {
            const std::string leader_name = platoon_names[i] + "-LEADER";
            const int leader_id = env->RegisterEntityIdByName(leader_name);
            PlatoonLeader* leader = new PlatoonLeader(engine, leader_id, &platoon_ids[i]);
            cmp_Ids.push_back(leader_id);
            leader->SetModelName(leader_name);
            leader->SetParentModel(this);
            this->RegisterSubModel(leader);
            platoon_leaders.push_back(leader);
        }

        Artillery* art = nullptr;
        if (hasScenario) {
            for (const auto& entity : scenario.entities) {
                if (entity.side != SideType::BLUE) continue;
                if (entity.forceType != ForceType::ARTILLERY) continue;
                const int id = env->RegisterEntityIdByName(entity.name);
                art = new Artillery(engine, Entity{id, entity.name, entity.side, entity.forceType, entity.position});
                cmp_Ids.push_back(id);
                art->SetParentModel(this);
                this->RegisterSubModel(art);
                break;
            }
        }

        // 중대장 - 중대원보다 나중에 생성할 것
        HQ* hq = new HQ(engine,&cmp_Ids);
        hq->SetModelName("BLUE-HQ");
        hq->SetParentModel(this);
        this->RegisterSubModel(hq);

        this->AddInputPort("Start");
        this->AddInputPort("RedFire");
        this->AddOutputPort("BlueFire");
        this->AddInputPort("RedPosition");
        this->AddOutputPort("BluePosition");

        // === 커플링 ===
        this->AddCoupling(this,"Start",hq,"Start",EIC);
        
        for (auto* leader : platoon_leaders) {
            this->AddCoupling(hq,"CompanyOrd",leader,"CompanyOrd",IC);
            this->AddCoupling(leader,"PlatoonRep",hq,"InfantryRep",IC);
        }
        if (art) {
            this->AddCoupling(hq,"CompanyOrd",art,"CompanyOrd",IC);
            this->AddCoupling(art,"FireOut",this,"BlueFire",EOC);
        }

        for (std::size_t p = 0; p < platoons.size(); ++p) {
            auto& platoon = platoons[p];
            auto* leader = platoon_leaders[p];
            for (auto* soldier : platoon) {
                this->AddCoupling(this, "Start", soldier, "Start", EIC);
                this->AddCoupling(this, "RedFire", soldier, "FireIn", EIC);
                this->AddCoupling(soldier, "FireOut", this, "BlueFire", EOC);
                this->AddCoupling(soldier, "PositionOut", this, "BluePosition", EOC);
                this->AddCoupling(this, "RedPosition", soldier, "PositionIn", EIC);
                this->AddCoupling(leader, "PlatoonOrd", soldier, "PlatoonOrd", IC);
                this->AddCoupling(soldier, "SoldierRep", leader, "SoldierRep", IC);
                this->AddCoupling(soldier, "FireOut", leader, "FireFinished", IC);
                if (art) {
                    this->AddCoupling(art, "FireOut", soldier, "FireIn", IC);
                }
            }
        }
    }
};
