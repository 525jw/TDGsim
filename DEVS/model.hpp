#pragma once
#include <vector>
#include <string>
#include <algorithm>
#include <typeinfo>

#define TIME_INF 1e30f
using TIME_T = float;
class Event;
class Engine;

class Model{
protected:
    // Timing Variables
    TIME_T lastTime = 0.0;
    mutable TIME_T nextTime = TIME_INF;

    Engine* engine = nullptr;
    Model* parentModel = nullptr;

private:
    int modelID = -1;
    // input and output event sets
    std::vector<std::string> inputPorts;
    std::vector<std::string> outputPorts;
    
public: 
    Model(Engine* engine);
    void SetModelID(int id);
    void SetEngine(Engine* engine);
    void SetParentModel(Model* parentModel);

    const int GetModelID() const;
    const Engine* GetEngine() const;
    Model* GetParentModel() const;

    const TIME_T GetNextTime() const;

    void AddInputPort(const std::string& inputPort);
    void AddOutputPort(const std::string& outputPort);
    void RemoveInputPort(const std::string& inputPort);
    void RemoveOutputPort(const std::string& outputPort);
    const std::vector<std::string>& GetInputPorts() const;
    const std::vector<std::string>& GetOutputPorts() const;

    virtual void ReceiveEvent(Event& event, TIME_T currentTime) = 0; // when receive (x,t) 
    virtual void ReceiveScheduleTime(const TIME_T currentTime) = 0; // when receive (*,t)

    virtual const TIME_T QueryNextTime() const = 0; // Recursively returns earliest nextTime from child models

    virtual bool IsAtomic() const { return false; }
    virtual bool IsCoupled() const { return false; }

    void LogMyBirth() const;
    virtual const char* ClassName() const {return typeid(*this).name();}

    virtual ~Model();
};
