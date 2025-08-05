#include "generator.hpp"
#include <ctime>

Generator::Generator(Engine* engine)
    :  AtomicModel(engine)
{
    this->AddState("START");
    this->AddState("WAIT");

    this->SetCurState("START");
    
    this->AddInputPort("restart");
    this->AddOutputPort("scenInfo");

    this->seed = GenerateSeed();
}

unsigned int Generator::GenerateSeed() {
    return static_cast<unsigned int>(time(nullptr));
}

bool Generator::ExtTransFn(const std::string& inPort, const std::any& anyMessage) {
    if(inPort == "restart" && this->GetCurState() == "WAIT"){
        // RestartMsg message;
        // if(!TryCastMessage(anyMessage, message, "Generator::ExtTransFn::restart")) return false;
        // this->SetCurState("START");
    }
    return true;
}

bool Generator::IntTransFn() {
    if(this->GetCurState()=="START"){
        this->SetCurState("WAIT");
    }
    return true;
}

bool Generator::OutputFn() {
    if (this->GetCurState() == "START") {
        ScenInfo message;
        message.seed = this->seed;
        /*
        ----------------------------------------------------------------------
        100 x 100 사이즈 4분면에 5명씩
        ----------------------------------------------------------------------
        */
        int unitsPerQuadrant = 5;
        int spacing = 2;

        auto generate = [&](int idStart, int xMin, int xMax, int yMin, int yMax, auto& teamMap) {
            std::uniform_int_distribution<int> distX(xMin, xMax);
            std::uniform_int_distribution<int> distY(yMin, yMax);

            int count = 0;
            while (count < unitsPerQuadrant) {
                int x = distX(this->seed);
                int y = distY(this->seed);

                // 중앙 횡방향 금지구역 (y in [48, 52])
                if (y >= 48 && y <= 52) continue;

                teamMap[idStart + count] = {x, y};
                count++;
            }
        };

        // RedTeam 1사분면 (x:0~49, y:0~49)
        generate(1001, 5, 45, 5, 45, message.redTeam);

        // RedTeam 2사분면 (x:50~99, y:0~49)
        generate(1006, 55, 95, 5, 45, message.redTeam);

        // BlueTeam 3사분면 (x:0~49, y:50~99)
        generate(2001, 5, 45, 55, 95, message.blueTeam);

        // BlueTeam 4사분면 (x:50~99, y:50~99)
        generate(2006, 55, 95, 55, 95, message.blueTeam);

        /*
        ----------------------------------------------------------------------
        */
        std::any anyMessage = message;
        this->AddOutputEvent("scenInfo",anyMessage);
    }
    return true;
}
TIME_T Generator::TimeAdvanceFn() {
    if (this->GetCurState() == "START") {
        return 0.0f;
    }else if(this->GetCurState()=="WAIT"){
        return TIME_INF;
    }
    return -1;
}