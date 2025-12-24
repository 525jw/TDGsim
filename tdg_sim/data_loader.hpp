#pragma once
#include <string>
#include <string_view>
#include <unordered_set>
#include <algorithm>
#include <cctype>
#include <cmath>
#include <fstream>
#include "common.hpp"
#include "tdg_sim/sim/environment/environment.hpp"

// parameters: (path, filter, output)
// returns: bool success
namespace data_loader {
    bool LoadScenarioFromFile(std::string_view path, Scenario& out);
    bool LoadGoalFromFile(std::string_view path, std::vector<Rect>& out);
    bool LoadOrderFromFile( std::string_view bmlPath, SideType sideFilter, CompanyOrd& out);
}
