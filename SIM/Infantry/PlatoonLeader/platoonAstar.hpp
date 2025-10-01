#pragma once
#include "SIM/Environment/environment.hpp"
#include <vector>
#include <unordered_map>
#include <string>

struct PointHash {
    std::size_t operator()(const Point& p) const noexcept {
        return (static_cast<std::size_t>(p.x) << 32) ^ static_cast<std::size_t>(p.y);
    }
};

struct PlatoonManeuverPlan {
    bool success = false;
    Point referenceStart{0, 0};
    Point goal{0, 0};
    std::vector<int> orderedMemberIds;
    std::unordered_map<int, Point> memberStartPositions;
    std::unordered_map<int, std::vector<Point>> memberPaths;
    std::string failureReason;
};

std::vector<Point> AStarPlan(Point start, Point goal, int max_expand = 200000);
PlatoonManeuverPlan BuildPlatoonManeuverPlan(const std::vector<int>& memberIds, Point desiredGoal, int max_expand = 200000);
