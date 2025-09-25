#pragma once
#include "SIM/Environment/environment.hpp"
#include <vector>
#include <queue>
#include <unordered_map>
#include <limits>
#include <cstdlib>

struct PointHash {
    std::size_t operator()(const Point& p) const noexcept {
        return (static_cast<std::size_t>(p.x) << 32) ^ static_cast<std::size_t>(p.y);
    }
};
std::vector<Point> AStarPlan(Point start, Point goal, int max_expand = 200000);
