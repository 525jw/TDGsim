/*
입력사건, 출력사건, 상태 및 인터페이스
상위 모델
*/
#pragma once

#include <vector>
#include <string>

class BasicModel{
private:
    std::vector<std::string> inputEvents;
    std::vector<std::string> outputEvents;
    std::vector<std::string> states;
    BasicModel* parentModel = nullptr;
    int modelID = 0;
    

public:
    void AddInputPort(const std::string& input);
    void AddOutputPort(const std::string& output);
    void RemoveInputPort(const std::string& input);
    void RemoveOutputPort(const std::string& output);
    void AddState(const std::string& state);
    void RemoveState(const std::string& state);
    void SetModelID(int id);

    const std::vector<std::string>& GetInputEvents() const;
    const std::vector<std::string>& GetOutputEvents() const;
    const std::vector<std::string>& GetStates() const;
    int GetModelID() const;
};