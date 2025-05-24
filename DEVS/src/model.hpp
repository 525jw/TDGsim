#pragma once
#include <vector>
#include <string>

#define TIME_INF 1e30f
using TIME_T = float;
class Engine;
class Event;

class Model{
protected:
    // Timing Variables
    TIME_T lastTime = 0.0;
    TIME_T nextTime = 0.0;

    Engine* engine = nullptr;
    Model* parentModel = nullptr;

private:
    int modelID;
    // input and output event sets
    std::vector<std::string> inputPorts;
    std::vector<std::string> outputPorts;
    
public: 
    Model(int modelID, Engine* engine);
    void SetModelID(int id);
    void SetEngine(Engine* engine);
    void SetParentModel(Model* parentModel);

    const int GetModelID() const;
    const Engine* GetEngine() const;
    const Model* GetParentModel() const;

    const TIME_T GetNextTime() const;

    void AddInputPort(const std::string& inputPort);
    void AddOutputPort(const std::string& outputPort);
    void RemoveInputPort(const std::string& inputPort);
    void RemoveOutputPort(const std::string& outputPort);
    const std::vector<std::string>& GetInputPorts() const;
    const std::vector<std::string>& GetOutputPorts() const;

    virtual void ReceiveEvent(Event& event, TIME_T currentTime) = 0; // when receive (x,t) 
    virtual void ReceiveScheduleTime(const TIME_T currentTime) = 0; // when receive (*,t)

    // virtual const TIME_T QueryNextTime() const = 0;

    virtual bool IsAtomic() const { return false; }
    virtual bool IsCoupled() const { return false; }

    virtual ~Model();
};