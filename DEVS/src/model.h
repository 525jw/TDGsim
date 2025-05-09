#pragma once
#include <vector>
#include <string>
#include <any>
#include <unordered_map>

class Engine;
class Event;

class Model{
private:
    std::vector<std::string> inputs;
    std::vector<std::string> outputs;
    std::unordered_map<std::string, std::any> states; // 딕셔너리 구조 대응
    
    Model* parentModel = nullptr;
    int modelID = 0;
    float time = 0.0f; 
      
protected:
    Engine* engine = nullptr;

public:
    // 포트 관리
    void AddInputPort(const std::string& input);
    void AddOutputPort(const std::string& output);
    void RemoveInputPort(const std::string& input);
    void RemoveOutputPort(const std::string& output);
    
    // 상태 관리
    void AddState(const std::string& state, const std::any& value);
    void RemoveState(const std::string& state);
    void SetStateValue(const std::string& state, const std::any& value);
    std::any GetStateValue(const std::string& state) const;

    // 묶어둠
    void SetParentModel(Model* model);
    const Model* GetParentModel() const;
    void SetModelID(int id);
    int GetModelID() const;

    // getter
    const std::vector<std::string>& GetInputEvents() const;
    const std::vector<std::string>& GetOutputEvents() const;
    const std::unordered_map<std::string, std::any>& GetStates() const;

    // 엔진, 시간
    void SetEngine(Engine* e);
    Engine* GetEngine() const;
    void SetTime(float t) { time = t; }
    float GetTime() const { return time; }

    virtual void HandleExtEvent(const Event& e, const std::string& port, float time) = 0;
    virtual void HandleTimeAdvance() = 0;

    virtual float GetNextTime() const = 0;
    virtual float GetNextTimeAdvance() const = 0;
    
    virtual ~Model() = default; // Virtual destructor for proper cleanup of derived classes
};