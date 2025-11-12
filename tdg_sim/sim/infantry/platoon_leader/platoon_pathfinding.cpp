#include "platoon_pathfinding.hpp"
#include <queue>
#include <algorithm>
#include <cmath>
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

    std::vector<Point> CollectCandidateGoals(Environment& environment,
                                             Point center,
                                             int requiredCount,
                                             int max_expand,
                                             std::string& failureReason) {
        std::vector<Point> result;
        if (requiredCount <= 0) {
            return result;
        }

        if (!environment.InBounds(center)) {
            center = ClampIntoBounds(environment, center);
        }

        const int width = environment.GetWidth();
        const int height = environment.GetHeight();
        if (width <= 0 || height <= 0) {
            failureReason = "Environment bounds are invalid.";
            return result;
        }

        const int limit = (max_expand <= 0)
            ? std::numeric_limits<int>::max()
            : max_expand;

        std::queue<Point> q;
        std::unordered_set<Point, PointHash> visited;
        visited.reserve(static_cast<std::size_t>(std::min<long long>(
            static_cast<long long>(limit),
            static_cast<long long>(width) * static_cast<long long>(height))));

        visited.insert(center);
        q.push(center);

        int expanded = 0;
        while (!q.empty() && expanded < limit &&
               static_cast<int>(result.size()) < requiredCount) {
            Point current = q.front();
            q.pop();
            ++expanded;

            if (TerrainPassable(environment, current)) {
                result.push_back(current);
            }

            for (const Point& dir : DIR4) {
                Point next{current.x + dir.x, current.y + dir.y};
                if (!environment.InBounds(next)) continue;
                if (!visited.insert(next).second) continue;
                q.push(next);
            }
        }

        if (static_cast<int>(result.size()) < requiredCount) {
            failureReason = "Not enough passable tiles near waypoint to place platoon members.";
        }
        return result;
    }

    bool FindPathToAnyTarget(Environment& environment,
                             Point start,
                             const std::unordered_set<Point, PointHash>& targets,
                             int max_expand,
                             int memberId,
                             Point& outTarget,
                             std::vector<Point>& outPath,
                             std::string& failureReason) {
        outPath.clear();
        outTarget = {-1, -1};

        if (!environment.InBounds(start)) {
            failureReason = "Start position out of bounds for member id " + std::to_string(memberId) + ".";
            return false;
        }
        if (targets.empty()) {
            failureReason = "No available goal tiles to assign.";
            return false;
        }

        const int width = environment.GetWidth();
        const int height = environment.GetHeight();
        if (width <= 0 || height <= 0) {
            failureReason = "Environment bounds are invalid.";
            return false;
        }

        auto indexOf = [&](Point p) -> std::size_t {
            return static_cast<std::size_t>(p.y) * static_cast<std::size_t>(width) +
                   static_cast<std::size_t>(p.x);
        };

        const int limit = (max_expand <= 0)
            ? std::numeric_limits<int>::max()
            : max_expand;

        std::vector<uint8_t> visited(static_cast<std::size_t>(width) * static_cast<std::size_t>(height), 0);
        std::vector<Point> parent(static_cast<std::size_t>(width) * static_cast<std::size_t>(height), Point{-1, -1});
        std::queue<Point> q;

        const std::size_t startIdx = indexOf(start);
        visited[startIdx] = 1;
        parent[startIdx] = start;
        q.push(start);

        int expanded = 0;
        while (!q.empty() && expanded < limit) {
            Point current = q.front();
            q.pop();
            ++expanded;

            if (targets.find(current) != targets.end()) {
                outTarget = current;

                std::vector<Point> reversed;
                Point step = current;
                while (true) {
                    reversed.push_back(step);
                    if (step == start) break;
                    std::size_t idx = indexOf(step);
                    Point prev = parent[idx];
                    if (prev.x == -1 && prev.y == -1) {
                        break;
                    }
                    step = prev;
                }
                if (reversed.back() != start) {
                    failureReason = "Failed to reconstruct path for member id " + std::to_string(memberId) + ".";
                    return false;
                }
                outPath.assign(reversed.rbegin(), reversed.rend());
                return true;
            }

            for (const Point& dir : DIR4) {
                Point neighbor{current.x + dir.x, current.y + dir.y};
                if (!TerrainPassable(environment, neighbor)) continue;
                std::size_t idx = indexOf(neighbor);
                if (visited[idx]) continue;
                visited[idx] = 1;
                parent[idx] = current;
                q.push(neighbor);
            }
        }

        failureReason = "Unable to route member id " + std::to_string(memberId) + " to an available goal tile.";
        return false;
    }

    bool ComputePathsForMembers(PlatoonManeuverPlan& plan,
                                Environment& environment,
                                Point waypointGoal,
                                std::string& failureReason) {
        plan.memberStartPositions.clear();
        plan.memberGoalPositions.clear();
        plan.memberPaths.clear();
        plan.memberPathIndices.clear();

        std::vector<Point> starts;
        starts.reserve(plan.orderedMemberIds.size());
        for (int id : plan.orderedMemberIds) {
            Point pos = environment.QueryEntityPosById(id);
            if (!environment.InBounds(pos)) {
                failureReason = "Invalid start position for member id " + std::to_string(id) + ".";
                return false;
            }
            plan.memberStartPositions.emplace(id, pos);
            starts.push_back(pos);
        }
        plan.referenceStart = ComputeReferencePoint(starts);

        std::vector<Point> candidateGoals = CollectCandidateGoals(
            environment,
            waypointGoal,
            static_cast<int>(plan.orderedMemberIds.size()),
            plan.maxExpand,
            failureReason);
        if (static_cast<int>(candidateGoals.size()) < static_cast<int>(plan.orderedMemberIds.size())) {
            return false;
        }

        std::unordered_set<Point, PointHash> availableTargets(candidateGoals.begin(), candidateGoals.end());

        struct MemberOrderInfo {
            int id;
            int heuristic;
        };
        std::vector<MemberOrderInfo> sortedMembers;
        sortedMembers.reserve(plan.orderedMemberIds.size());
        for (int id : plan.orderedMemberIds) {
            Point pos = plan.memberStartPositions[id];
            int heuristic = std::abs(pos.x - waypointGoal.x) + std::abs(pos.y - waypointGoal.y);
            sortedMembers.push_back(MemberOrderInfo{id, heuristic});
        }
        std::sort(sortedMembers.begin(), sortedMembers.end(),
                  [](const MemberOrderInfo& lhs, const MemberOrderInfo& rhs) {
                      if (lhs.heuristic != rhs.heuristic) return lhs.heuristic < rhs.heuristic;
                      return lhs.id < rhs.id;
                  });

        for (const MemberOrderInfo& info : sortedMembers) {
            if (availableTargets.empty()) {
                failureReason = "No remaining goal tiles while assigning member id " + std::to_string(info.id) + ".";
                return false;
            }

            Point start = plan.memberStartPositions[info.id];
            Point target;
            std::vector<Point> path;
            if (!FindPathToAnyTarget(environment,
                                     start,
                                     availableTargets,
                                     plan.maxExpand,
                                     info.id,
                                     target,
                                     path,
                                     failureReason)) {
                return false;
            }

            availableTargets.erase(target);
            plan.memberGoalPositions.emplace(info.id, target);
            plan.memberPaths.emplace(info.id, path);

            std::size_t currentIndex = 0;
            if (!path.empty() && path.front() != start) {
                auto found = std::find(path.begin(), path.end(), start);
                if (found != path.end()) {
                    currentIndex = static_cast<std::size_t>(std::distance(path.begin(), found));
                } else {
                    path.insert(path.begin(), start);
                    plan.memberPaths[info.id] = path;
                    currentIndex = 0;
                }
            }
            plan.memberPathIndices.emplace(info.id, currentIndex);
        }

        failureReason.clear();
        return true;
    }
}

PlatoonManeuverPlan BuildPlatoonManeuverPlan(
    const std::vector<int>& memberIds,
    Point desiredGoal,
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

    Point sanitizedGoal = FindNearestPassable(environment, desiredGoal, max_expand);
    if (!environment.InBounds(sanitizedGoal) || !TerrainPassable(environment, sanitizedGoal)) {
        plan.failureReason = "Unable to resolve waypoint in bounds.";
        return plan;
    }

    plan.waypointGoals.clear();
    plan.waypointGoals.push_back(sanitizedGoal);

    plan.goal = sanitizedGoal;
    plan.currentGoal = sanitizedGoal;
    plan.activeWaypoint = 0;

    if (!ComputePathsForMembers(plan, environment, plan.currentGoal, plan.failureReason)) {
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
    if (plan.waypointGoals.empty() ||
        plan.orderedMemberIds.empty() ||
        waypointIndex >= plan.waypointGoals.size()) {
        plan.failureReason = "Waypoint index out of range.";
        plan.success = false;
        return false;
    }

    Environment& environment = *env;
    plan.activeWaypoint = waypointIndex;
    plan.currentGoal = plan.waypointGoals[plan.activeWaypoint];

    if (!ComputePathsForMembers(plan, environment, plan.currentGoal, plan.failureReason)) {
        plan.success = false;
        return false;
    }

    plan.success = true;
    plan.failureReason.clear();
    return true;
}
