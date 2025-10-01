#include "platoonAstar.hpp"
#include <queue>
#include <cmath>
#include <limits>
#include <unordered_set>
#include <algorithm>
#include <cstdlib>

namespace {
    constexpr int STRAIGHT_COST = 10;
    constexpr int DIAGONAL_COST = 14;

    const Point DIR8[8] = {
        {+1, 0}, {-1, 0}, {0, +1}, {0, -1},
        {+1, +1}, {+1, -1}, {-1, +1}, {-1, -1}
    };

    inline Point Add(Point a, Point b) noexcept { return {a.x + b.x, a.y + b.y}; }

    inline int Manhattan(Point a, Point b) noexcept {
        return std::abs(a.x - b.x) + std::abs(a.y - b.y);
    }

    inline int DiagonalHeuristic(Point a, Point b) noexcept {
        int dx = std::abs(a.x - b.x);
        int dy = std::abs(a.y - b.y);
        int diag = std::min(dx, dy);
        int straight = std::max(dx, dy) - diag;
        return diag * DIAGONAL_COST + straight * STRAIGHT_COST;
    }

    inline bool TerrainPassable(Point p) {
        if (!EnvReady()) return false;
        Environment& environment = *env;
        if (!environment.InBounds(p)) return false;
        return environment.GetTerrainAt(p) != TerrainType::RIVER;
    }

    std::vector<Point> ReconstructPath(
        Point start,
        Point goal,
        const std::unordered_map<Point, Point, PointHash>& parent) {
        std::vector<Point> path;
        Point current = goal;
        while (current != start) {
            path.push_back(current);
            auto it = parent.find(current);
            if (it == parent.end()) {
                path.clear();
                return path;
            }
            current = it->second;
        }
        std::reverse(path.begin(), path.end());
        return path;
    }

    template <class PassableFn>
    std::vector<Point> RunAStar(Point start, Point goal, PassableFn passable,
                                int max_expand) {
        std::vector<Point> empty;

        if (max_expand <= 0) {
            max_expand = std::numeric_limits<int>::max();
        }

        if (!passable(start) || !passable(goal)) {
            return empty;
        }

        struct Node {
            Point p;
            int g;
            int f;
        };
        struct Compare {
            bool operator()(const Node& a, const Node& b) const {
                if (a.f == b.f) return a.g < b.g;
                return a.f > b.f;
            }
        };

        std::priority_queue<Node, std::vector<Node>, Compare> open;
        std::unordered_map<Point, int, PointHash> gScore;
        std::unordered_map<Point, Point, PointHash> parent;
        std::unordered_set<Point, PointHash> closed;

        gScore[start] = 0;
        open.push(Node{start, 0, DiagonalHeuristic(start, goal)});

        int expanded = 0;
        while (!open.empty()) {
            Node current = open.top();
            open.pop();

            if (current.p == goal) {
                return ReconstructPath(start, goal, parent);
            }

            if (closed.find(current.p) != closed.end()) {
                continue;
            }
            closed.insert(current.p);

            if (++expanded > max_expand) {
                break;
            }

            for (const Point& dir : DIR8) {
                Point next = Add(current.p, dir);
                if (!passable(next)) {
                    continue;
                }

                bool diagonal = (dir.x != 0 && dir.y != 0);
                int stepCost = diagonal ? DIAGONAL_COST : STRAIGHT_COST;
                int tentativeG = current.g + stepCost;

                auto it = gScore.find(next);
                if (it != gScore.end() && tentativeG >= it->second) {
                    continue;
                }

                gScore[next] = tentativeG;
                parent[next] = current.p;
                int heuristic = DiagonalHeuristic(next, goal);
                open.push(Node{next, tentativeG, tentativeG + heuristic});
            }
        }

        return empty;
    }

    Point ComputeReference(const std::vector<Point>& positions) {
        if (positions.empty()) {
            return {0, 0};
        }
        long long sumX = 0;
        long long sumY = 0;
        for (const Point& p : positions) {
            sumX += p.x;
            sumY += p.y;
        }
        const long long count = static_cast<long long>(positions.size());
        int refX = static_cast<int>(sumX / count);
        int refY = static_cast<int>(sumY / count);
        return {refX, refY};
    }
}

std::vector<Point> AStarPlan(Point start, Point goal, int max_expand) {
    auto passable = [&](Point p) {
        return TerrainPassable(p);
    };
    return RunAStar(start, goal, passable, max_expand);
}

PlatoonManeuverPlan BuildPlatoonManeuverPlan(const std::vector<int>& memberIds,
                                               Point desiredGoal,
                                               int max_expand) {
    PlatoonManeuverPlan plan;
    plan.goal = desiredGoal;
    plan.orderedMemberIds = memberIds;

    if (!EnvReady()) {
        plan.failureReason = "Environment is not ready.";
        return plan;
    }

    Environment& environment = *env;

    if (memberIds.empty()) {
        plan.failureReason = "No platoon members provided.";
        return plan;
    }

    if (!environment.InBounds(desiredGoal)) {
        plan.failureReason = "Goal is out of bounds.";
        return plan;
    }
    if (environment.GetTerrainAt(desiredGoal) == TerrainType::RIVER) {
        plan.failureReason = "Goal terrain is impassable.";
        return plan;
    }

    std::vector<Point> starts;
    starts.reserve(memberIds.size());

    for (int id : memberIds) {
        Point pos = environment.QueryEntityPosById(id);
        if (pos.x < 0 || pos.y < 0) {
            plan.failureReason = "Unable to query position for member id " + std::to_string(id) + ".";
            return plan;
        }
        plan.memberStartPositions.emplace(id, pos);
        starts.push_back(pos);
    }

    plan.referenceStart = ComputeReference(starts);

    for (std::size_t i = 0; i < memberIds.size(); ++i) {
        int id = memberIds[i];
        Point start = starts[i];
        std::vector<Point> path = AStarPlan(start, desiredGoal, max_expand);

        if (path.empty() && start != desiredGoal) {
            plan.failureReason = "Unable to build path for member id " + std::to_string(id) + ".";
            plan.memberPaths.clear();
            plan.success = false;
            return plan;
        }

        plan.memberPaths.emplace(id, std::move(path));
    }

    plan.success = true;
    return plan;
}
