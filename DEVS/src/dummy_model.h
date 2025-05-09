#pragma once
#include "atomic_model.h"
#include <iostream>
#include <string>
#include <any>

class DummyModel : public AtomicModel {
public:
    DummyModel(int id) : AtomicModel(id) {
        AddInputPort("in");
        AddOutputPort("out");
    }

    float TimeAdvanceFn() override {
        return 1.0f; // 매 1초마다 발사
    }

    bool OutputFn() override {
        std::cout << "[DummyModel " << GetModelID() << "] Output triggered\n";
        AddOutputEvent("out", std::string("ping"));
        return true;
    }

    bool IntTransFn() override {
        return true;
    }

    bool ExtTransFn(const std::string& port, const std::any& msg) override {
        std::cout << "[DummyModel " << GetModelID() << "] Received on port '"
                  << port << "': " << std::any_cast<std::string>(msg) << "\n";
        return true;
    }
};
