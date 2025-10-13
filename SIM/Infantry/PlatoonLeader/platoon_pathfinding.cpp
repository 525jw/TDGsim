#include "platoon_pathfinding.hpp"
#include <queue>
#include <algorithm>
#include <limits>
#include <unordered_set>

namespace {
    constexpr Point DIR4[4] = {
        {+1, 0}, {-1, 0}, {0, +1}, {0, -1}
    };

    inline bool TerrainPassable(Environment& environment, Point p) {
        return environment.InBounds(p) &&
               environment.GetTerrainAt(p) != TerrainType::RIVER;
    }

    Point ClampIntoBounds(const Environment& environment, Point p) {
        return {
            std::clamp(p.x, 0, environment.GetWidth() - 1),
            std::clamp(p.y, 0, environment.GetHeight() - 1)
        };
    }

    Point FindNearestPassable(Environment& environment,
                              Point desired,
                              int max_expand) {
        const int width = environment.GetWidth();
        const int height = environment.GetHeight();
        if (width <= 0 || height <= 0) {
            return {-1, -1};
        }

        const int limit = (max_expand <= 0)
            ? std::numeric_limits<int>::max()
            : max_expand;

        std::queue<Point> q;
        std::unordered_set<Point, PointHash> visited;
        const long long totalCells =
            static_cast<long long>(width) * static_cast<long long>(height);
        const long long reserveCount = std::min<long long>(limit, totalCells);
        if (reserveCount > 0) {
            visited.reserve(static_cast<std::size_t>(reserveCount));
        }

        Point start = desired;
        if (!environment.InBounds(start)) {
            start = ClampIntoBounds(environment, start);
        }

        visited.insert(start);
        q.push(start);

        int expanded = 0;
        while (!q.empty() && expanded < limit) {
            Point current = q.front();
            q.pop();
            ++expanded;

            if (TerrainPassable(environment, current)) {
                return current;
            }

            for (const Point& dir : DIR4) {
                Point next{current.x + dir.x, current.y + dir.y};
                if (!environment.InBounds(next)) continue;
                if (!visited.insert(next).second) continue;
                q.push(next);
            }
        }

        return {-1, -1};
    }

    Point ComputeReferencePoint(const std::vector<Point>& points) {
        if (points.empty()) {
            return {0, 0};
        }
        long long sumX = 0;
        long long sumY = 0;
        for (const Point& p : points) {
            sumX += p.x;
            sumY += p.y;
        }
        const long long count = static_cast<long long>(points.size());
        return {
            static_cast<int>(sumX / count),
            static_cast<int>(sumY / count)
        };
    }

    bool PopulateNextStepGrid(PlatoonManeuverPlan& plan,
                              Environment& environment,
                              Point goal,
                              int max_expand,
                              std::string& failureReason) {
        if (!environment.InBounds(goal) || !TerrainPassable(environment, goal)) {
            failureReason = "Waypoint out of bounds or impassable.";
            return false;
        }

        const int width = environment.GetWidth();
        const int height = environment.GetHeight();
        plan.gridWidth = width;
        plan.gridHeight = height;
        plan.nextStepGrid.assign(static_cast<std::size_t>(width) * static_cast<std::size_t>(height),
                                 Point{-1, -1});

        auto indexOf = [&](Point p) -> std::size_t {
            return static_cast<std::size_t>(p.y) * static_cast<std::size_t>(width) +
                   static_cast<std::size_t>(p.x);
        };

        std::vector<uint8_t> visited(static_cast<std::size_t>(width) * static_cast<std::size_t>(height), 0);
        const int limit = (max_expand <= 0)
            ? std::numeric_limits<int>::max()
            : max_expand;

        std::queue<Point> q;
        visited[indexOf(goal)] = 1;
        plan.nextStepGrid[indexOf(goal)] = goal;
        q.push(goal);

        int expanded = 0;
        while (!q.empty() && expanded < limit) {
            Point current = q.front();
            q.pop();
            ++expanded;

            for (const Point& dir : DIR4) {
                Point neighbor{current.x + dir.x, current.y + dir.y};
                if (!TerrainPassable(environment, neighbor)) continue;
                std::size_t idx = indexOf(neighbor);
                if (visited[idx]) continue;
                visited[idx] = 1;
                plan.nextStepGrid[idx] = current;
                q.push(neighbor);
            }
        }

        for (int id : plan.orderedMemberIds) {
            Point pos = environment.QueryEntityPosById(id);
            if (!environment.InBounds(pos)) {
                failureReason = "Invalid current position for member id " + std::to_string(id) + ".";
                return false;
            }
            if (pos != goal) {
                std::size_t idx = indexOf(pos);
                Point next = plan.nextStepGrid[idx];
                if (next.x == -1 && next.y == -1) {
                    failureReason = "No traversable path for member id " + std::to_string(id) + ".";
                    return false;
                }
            }
        }

        plan.failureReason.clear();
        return true;
    }
}

PlatoonManeuverPlan BuildPlatoonManeuverPlan(
    const std::vector<int>& memberIds,
    Point desiredGoal,
    const std::vector<Point>& route,
    int max_expand) {
    PlatoonManeuverPlan plan;
    plan.orderedMemberIds = memberIds;
    plan.goal = desiredGoal;
    plan.maxExpand = max_expand;

    if (!EnvReady()) {
        plan.failureReason = "Environment is not ready.";
        return plan;
    }

    Environment& environment = *env;

    if (memberIds.empty()) {
        plan.failureReason = "No platoon members provided.";
        return plan;
    }

    std::vector<Point> routeSequence = route;
    if (routeSequence.empty()) {
        routeSequence.push_back(desiredGoal);
    } else if (routeSequence.back() != desiredGoal) {
        routeSequence.push_back(desiredGoal);
    }

    plan.waypointGoals.clear();
    plan.waypointGoals.reserve(routeSequence.size());
    for (const Point& rawTarget : routeSequence) {
        Point sanitized = FindNearestPassable(environment, rawTarget, max_expand);
        if (!environment.InBounds(sanitized) || !TerrainPassable(environment, sanitized)) {
            plan.failureReason = "Unable to resolve waypoint in bounds.";
            plan.waypointGoals.clear();
            return plan;
        }
        plan.waypointGoals.push_back(sanitized);
    }

    plan.goal = plan.waypointGoals.back();
    plan.currentGoal = plan.waypointGoals.front();
    plan.activeWaypoint = 0;

    std::vector<Point> starts;
    starts.reserve(memberIds.size());
    plan.memberStartPositions.reserve(memberIds.size());
    for (int id : memberIds) {
        Point pos = environment.QueryEntityPosById(id);
        if (!environment.InBounds(pos)) {
            plan.failureReason = "Invalid start position for member id " + std::to_string(id) + ".";
            plan.memberStartPositions.clear();
            plan.waypointGoals.clear();
            return plan;
        }
        plan.memberStartPositions.emplace(id, pos);
        starts.push_back(pos);
    }

    plan.referenceStart = ComputeReferencePoint(starts);

    if (!PopulateNextStepGrid(plan, environment, plan.currentGoal, plan.maxExpand, plan.failureReason)) {
        plan.success = false;
        return plan;
    }

    plan.success = true;
    plan.failureReason.clear();
    return plan;
}

bool RebuildPlatoonWaypointPlan(
    PlatoonManeuverPlan& plan,
    std::size_t waypointIndex) {
    if (!EnvReady()) {
        plan.failureReason = "Environment is not ready.";
        plan.success = false;
        return false;
    }
    if (plan.waypointGoals.empty() || waypointIndex >= plan.waypointGoals.size()) {
        plan.failureReason = "Waypoint index out of range.";
        plan.success = false;
        return false;
    }

    Environment& environment = *env;
    plan.activeWaypoint = waypointIndex;
    plan.currentGoal = plan.waypointGoals[plan.activeWaypoint];

    if (!PopulateNextStepGrid(plan, environment, plan.currentGoal, plan.maxExpand, plan.failureReason)) {
        plan.success = false;
        return false;
    }

    plan.success = true;
    return true;
}
