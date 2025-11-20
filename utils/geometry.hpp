#pragma once
#include <vector>
#include <algorithm>

struct Point { int x; int y;};
inline bool operator==(const Point& a, const Point& b) {
    return a.x == b.x && a.y == b.y;
}
inline bool operator!=(const Point& a, const Point& b) {
    return !(a == b);
}
struct Rect { 
    int x1, y1; int x2, y2;
    bool Contains(Point p) const noexcept {
        return (p.x >= x1 && p.x <= x2 && p.y >= y1 && p.y <= y2);
    }
};
class Circle { 
    Point center; int radius;
    bool Contains(Point p) const noexcept {
        const int dx = p.x - center.x;
        const int dy = p.y - center.y;
        return (dx * dx + dy * dy <= radius * radius);
    }
    std::vector<Point> CellsIn(const Rect& clip) const {
        std::vector<Point> cells;
        const int x_start = std::max(clip.x1, center.x - radius);
        const int x_end   = std::min(clip.x2, center.x + radius);
        const int y_start = std::max(clip.y1, center.y - radius);
        const int y_end   = std::min(clip.y2, center.y + radius);

        for (int y = y_start; y <= y_end; ++y) {
            for (int x = x_start; x <= x_end; ++x) {
                Point p{x, y};
                if (Contains(p)) {
                    cells.push_back(p);
                }
            }
        }
        return cells;
    }
};
struct Line { 
    Point a; Point b;
    std::vector<Point> CellsOn(const Rect& clip){
        std::vector<Point> cells;
        int x1 = a.x, y1 = a.y;
        int x2 = b.x, y2 = b.y;

        const int dx = std::abs(x2 - x1);
        const int dy = std::abs(y2 - y1);
        const int sx = (x1 < x2) ? 1 : -1;
        const int sy = (y1 < y2) ? 1 : -1;
        int err = dx - dy;

        while (true) {
            Point p{x1, y1};
            if (clip.Contains(p)) {
                cells.push_back(p);
            }
            if (x1 == x2 && y1 == y2) break;
            int err2 = 2 * err;
            if (err2 > -dy) {
                err -= dy;
                x1 += sx;
            }
            if (err2 < dx) {
                err += dx;
                y1 += sy;
            }
        }
        return cells;
    }
};