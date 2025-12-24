#pragma once
#include "tdg_sim/sim/environment/environment.hpp"
#include <unordered_map>
#include <vector>
#include <string>

struct PointHash {
    std::size_t operator()(const Point& p) const noexcept {
        return (static_cast<std::size_t>(p.x) << 32) ^ static_cast<std::size_t>(p.y);
    }
};

struct PlatoonManeuverPlan {
    bool success = false;
    Point referenceStart{0, 0};
    Point goal{0, 0};         // representative wayPoint for the platoon
    Point currentGoal{0, 0};  // active wayPoint (same as goal for single-hop plans)
    std::vector<int> orderedMemberIds;
    std::unordered_map<int, Point> memberStartPositions;
    std::unordered_map<int, Point> memberGoalPositions;
    std::unordered_map<int, std::vector<Point>> memberPaths;
    std::unordered_map<int, std::size_t> memberPathIndices;
    std::vector<Point> wayPointGoals;
    std::size_t activeWayPoint = 0;
    std::string failureReason;
    int maxExpand = 200000;
};

// returns a PlatoonManeuverPlan with success=false on failure
PlatoonManeuverPlan BuildPlatoonManeuverPlan(
    const std::vector<int>& memberIds,
    Point desiredGoal,
    int max_expand = 200000);

bool RebuildPlatoonWayPointPlan(
    PlatoonManeuverPlan& plan,
    std::size_t wayPointIndex);

