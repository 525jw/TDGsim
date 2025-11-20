#include "generator.hpp"

Generator::Generator(Engine* engine)
    :  AtomicModel(engine)
{
    this->AddState("GENERATE");
    this->AddState("WAIT");

    this->SetCurState("GENERATE");
    
    this->AddInputPort("Restart");
    this->AddOutputPort("Start");

    seed_ = GenerateRandomSeed();
    this->UpdateTime(0.0f); // TODO : In dynamic DEVS, this value can be modified later
}

unsigned int Generator::GenerateRandomSeed() {
    auto now = std::chrono::high_resolution_clock::now();
    return static_cast<unsigned int>(
        now.time_since_epoch().count()
    );
}

bool Generator::ExtTransFn(const std::string& inPort, const std::any& anyMessage) {
    if(inPort == "Restart" && this->GetCurState() == "WAIT"){
        RestartMsg msg;
        if (!TryCastMessage(anyMessage, msg, "Generator::ExtTransFn.Restart")) return false;

        if (msg.needChangeSeed) seed_ = GenerateRandomSeed();
        if (msg.needChangeScenario)

        this->SetCurState("GENERATE");
    }
    return true;
}
bool Generator::OutputFn() {
    if (this->GetCurState() == "GENERATE") {
        if(!LoadScenarioFromJson(scenarioPath_, scenario_)){
            // error
        }
        if(scenario_.seed == 0 ) scenario_.seed = seed_;

        StartMsg message;
        message.scen = &scenario_;     
        std::any anyMessage = message;
        this->AddOutputEvent("Start",anyMessage);
    }
    return true;
}
bool Generator::IntTransFn() {
    if(this->GetCurState()=="GENERATE"){
        this->SetCurState("WAIT");
    }
    return true;
}

float Generator::TimeAdvanceFn() {
    if (this->GetCurState() == "GENERATE") return 0.0f;
    if(this->GetCurState()=="WAIT") return TIME_INF;
    return -1;
}

/*
── JSON 로드 ─────────────────────────────────────────────────────────
- nlohmann::json 사용
- 시나리오 파일에서 맵 크기, 지형, 엔티티 정보를 읽어 Scenario 구조체에 채움
- parseTerrainType, parseSide, parseForceType 헬퍼 함수 사용
*/
static std::string to_lower(std::string s){ // 대문자->소문자
    for(char& c : s) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return s;
}
static TerrainType parseTerrainType(const std::string& s){
    std::string k = to_lower(s);
    if (k == "river" || k == "water") return TerrainType::RIVER;
    return TerrainType::PLAIN;
}
static SideType parseSide(const std::string& s){
    std::string k = to_lower(s);
    return (k == "red") ? SideType::RED : SideType::BLUE;
}
static ForceType parseForceType(const std::string& s){
    std::string k = to_lower(s);
    if (k == "tank")      return ForceType::TANK;
    if (k == "artillery" || k == "art") return ForceType::ARTILLERY;
    return ForceType::RIFLE;
}
bool Generator::LoadScenarioFromJson(const std::string& path, Scenario& out){
    std::ifstream ifs(path);
    if (!ifs.is_open()){
        // error
        return false;
    }
    json j; ifs >> j;

    out.width  = j.contains("w") ? j["w"].get<int>()
              : (j.contains("width")  ? j["width"].get<int>()  : 0);
    out.height = j.contains("h") ? j["h"].get<int>()
              : (j.contains("height") ? j["height"].get<int>() : 0);
    if (out.width <= 0 || out.height <= 0){
        // error
        return false;
    }

    // seed(없으면 0)
    out.seed = j.value("seed", 0u);

    // 지형 사각형
    out.terrainRects.clear();
    for (const char* key : {"patches", "terrainRects"}) {
        if (!j.contains(key) || !j[key].is_array()) continue;
        for (const auto& pj : j[key]){
            TerrainType t = TerrainType::PLAIN;
            if (pj.contains("kind") && pj["kind"].is_string()){
                std::string kind = pj["kind"].get<std::string>();
                t = parseTerrainType(kind);
            }

            int x1 = pj.value("x1", 0);
            int y1 = pj.value("y1", 0);
            int x2 = pj.value("x2", x1);
            int y2 = pj.value("y2", y1);
            if (x1 > x2) std::swap(x1,x2);
            if (y1 > y2) std::swap(y1,y2);
            x1 = std::max(0, x1); y1 = std::max(0, y1);
            x2 = std::min(out.width  - 1, x2);
            y2 = std::min(out.height - 1, y2);

            TerrainRect tr{ t, x1, y1, x2, y2 };
            out.terrainRects.push_back(tr);
        }
        break;
    }

    // 엔티티: "units" 또는 "entities"
    out.entities.clear();
    const char* keysEnt[] = {"units", "entities"};
    for (const char* key : keysEnt){
        if (!j.contains(key) || !j[key].is_array()) continue;

        for (const auto& u : j[key]){
            Entity e{};
            e.name = u.value("uid", std::string{});

            e.side = parseSide(u.value("side", std::string{"BLUE"}));
            e.forceType = parseForceType(u.value("type", std::string{"rifle"}));

            // 좌표: x/y 또는 pos[2]
            if (u.contains("x") && u.contains("y")){
                e.position = { u["x"].get<int>(), u["y"].get<int>() };
            } else if (u.contains("pos") && u["pos"].is_array() && u["pos"].size() >= 2){
                e.position = { u["pos"][0].get<int>(), u["pos"][1].get<int>() };
            } else {
                e.position = {0,0};
            }

            out.entities.push_back(std::move(e));
        }
        break;
    }
    return true;
}