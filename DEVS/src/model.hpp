#pragma once
#include <vector>
#include <string>

#define TIME_INF 1e30f
using TIME_T = float;
class Engine;
class Event;

class Model{
protected:
    Engine* engine = nullptr;

    // Timing Variables
    TIME_T lastTime = 0.0;
    TIME_T nextTime = 0.0;

private:
    int modelID;
    
    // input and output event sets
    std::vector<std::string> inputPorts;
    std::vector<std::string> outputPorts;
    
public: 
    // Model(int modelID, Engine* engine);
    void SetModelID(int id);
    void SetEngine(Engine* engine);
    const int GetModelID() const;
    const Engine* GetEngine() const;
    
    void AddInputPort(const std::string& inputPort);
    void AddOutputPort(const std::string& outputPort);
    void RemoveInputPort(const std::string& inputPort);
    void RemoveOutputPort(const std::string& outputPort);
    const std::vector<std::string>& GetInputPorts() const;
    const std::vector<std::string>& GetOutputPorts() const;
    
    virtual void ReceiveExternalEvent(const Event& externalEvent, TIME_T engineTime) = 0; // when receive (x,t) 
    virtual void ReceiveTimeAdvanceRequest(const TIME_T engineTime) = 0; // when receive (*,t)
    virtual const TIME_T QueryNextTime() const = 0;
    virtual ~Model();
};