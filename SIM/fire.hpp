#pragma once
#include "DEVS/atomic_model.hpp"
#include "message.hpp"
#include "DEVS/logger.hpp"
#include <utility>
#include <iostream>

struct Capability {
    std::string weapon_name;
    int weapon_range;
    float hit_ratio;
    float reload_time;

    Capability(const std::string& name, int range, float ratio, float reload_time)
        : weapon_name(name), weapon_range(range), hit_ratio(ratio), reload_time(reload_time){}
};

class Fire : public AtomicModel{
private:
    int objectID;

    int targetID;
    std::vector<Capability> capability = { Capability{"rifle", 9, 0.7f, 2.0f} };

    TIME_T t_fire = -1.0f;
    TIME_T fireEquation();
public:
    Fire(int modelID, Engine* engine, int objectID);

    bool ExtTransFn(const std::string& inPort, const std::any& message);
    bool IntTransFn();
    bool OutputFn();
    TIME_T TimeAdvanceFn();

    void UpdateTime(const TIME_T currentTime);
    void ReceiveScheduleTime(const TIME_T currentTime) override;
    void ReceiveEvent(Event& event,TIME_T currentTime) override;
    const TIME_T QueryNextTime() const override;
};