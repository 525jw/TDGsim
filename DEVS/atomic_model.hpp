#pragma once
#include "model.hpp"
#include "event.hpp"
#include "engine.hpp"
#include "logger.hpp"
#include <algorithm>
#include <any>
#include <string>

class Engine;

class AtomicModel : public Model{
protected:
    TIME_T executedTime; // NOTE : Currently unused; specified in the textbook;
    template<typename T>
    static bool TryCastMessage(const std::any& raw, T& typed, const std::string& context = "") {
        try {
            typed = std::any_cast<T>(raw);
            return true;
        } catch (const std::bad_any_cast&) {
            std::cerr << "[ERROR] Invalid message type in " << context << std::endl;
            return false;
        }
    }
private:
    std::vector<std::string> states;
    std::string currentState;
public:
    AtomicModel(Engine* engine);

    void AddState(const std::string& state);
    void RemoveState(const std::string& state);
    const std::vector<std::string>& GetStates() const;
    void SetCurState(std::string state);
    const std::string& GetCurState() const;

    void ReceiveEvent(Event& event, TIME_T currentTime);
    void ReceiveScheduleTime(const TIME_T currentTime);
    const TIME_T QueryNextTime() const;

    void UpdateTime(const TIME_T currentTime);
    void AddOutputEvent(const std::string& outputPort, std::any& message);

    virtual bool ExtTransFn(const std::string& inPort, const std::any& anyMessage) {return false;}
    virtual bool IntTransFn() {return false;}
    virtual bool OutputFn() {return false;}
    virtual TIME_T TimeAdvanceFn() {return -1;}

    bool IsAtomic() const override { return true; }

};