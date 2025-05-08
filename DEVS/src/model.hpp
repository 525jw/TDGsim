#pragma once
#include <vector>
#include <string>
#include <vector>

#define TIME_T float
#define INF_FLOAT 1e30f

class Engine;
class Event;


class Model{
protected:
    TIME_T time;
private:
    std::vector<std::string> inputs;
    std::vector<std::string> outputs;
    std::vector<std::string> states;
    std::string curState;
    Model* parentModel = nullptr;
    int modelID = 0;
    Engine* engine = nullptr;
public:
    void AddInputPort(const std::string& input);
    void AddOutputPort(const std::string& output);
    void AddState(const std::string& state);

    void RemoveInputPort(const std::string& input);
    void RemoveOutputPort(const std::string& output);
    void RemoveState(const std::string& state);
    
    void SetParentModel(Model& model);
    void SetModelID(int id);
    void SetEngine(Engine& engine);
    void SetCurState(std::string state);

    const std::vector<std::string>& GetInputEvents() const;
    const std::vector<std::string>& GetOutputEvents() const;
    const std::vector<std::string>& GetStates() const;
    const std::string& GetCurState() const;
    const Model* GetParentModel() const;
    const int GetModelID() const;
    const Engine* GetEngine() const;

    virtual void HandleExtEvent(const Event extEvent, const std::string inPort, TIME_T engineTime);
    virtual void HandleTimeAdvance(const TIME_T engineTime);
    virtual const TIME_T queryNextTime() const;

    // const bool IsAtomic() const; // Used as isInstance alternative
};