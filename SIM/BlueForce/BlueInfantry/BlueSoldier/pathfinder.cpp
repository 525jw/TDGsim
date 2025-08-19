// A* Pathfinding Algorithm
#include "environment.hpp"
#include <vector>
#include <queue>
#include <unordered_set>
#include <cmath>
#include <algorithm>


struct Node {
    int x, y;
    float gCost; // 시작점에서 현재 노드까지의 비용
    float hCost; // 휴리스틱(목표까지의 예상 비용)
    float fCost() const { return gCost + hCost; }
    Node* parent;

    Node(int x, int y, float g = 0, float h = 0, Node* p = nullptr)
        : x(x), y(y), gCost(g), hCost(h), parent(p) {}
};

struct NodeCompare {
    bool operator()(const Node* a, const Node* b) const {
        return a->fCost() > b->fCost();
    }
};

float Heuristic(int x1, int y1, int x2, int y2) {
    // 맨해튼 거리
    return std::abs(x1 - x2) + std::abs(y1 - y2);
}


// 8방향 이동을 고려함
std::vector<std::pair<int, int>> GetNeighbors(int x, int y, int width, int height) {
    std::vector<std::pair<int, int>> neighbors;
    // 8방향 이동 (상하좌우 + 대각선)
    for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            if (dx == 0 && dy == 0) continue;   // 현재 위치라서 제외
            int nx = x + dx;
            int ny = y + dy;
            if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                neighbors.emplace_back(nx, ny);
            }
        }
    }
    return neighbors;
}

// A* How it works
// input: 2D grid map, start position, goal position
// output: path from start to goal vector of pairs (x, y)
// map: 0 = free, -1 = obstacle <- 이건 나중에 맵 정의에 따라 달라질 수 있음

std::vector<std::pair<int, int>> AStar(
    const std::vector<std::vector<int>>& map,
    std::pair<int, int> start,
    std::pair<int, int> goal
) {
    int width = map[0].size();
    int height = map.size();

    std::priority_queue<Node*, std::vector<Node*>, NodeCompare> openList;
    std::unordered_set<int> closedSet;

    auto nodeIndex = [width](int x, int y) { return y * width + x; };

    Node* startNode = new Node(start.first, start.second, 0, Heuristic(start.first, start.second, goal.first, goal.second));
    openList.push(startNode);

    std::vector<Node*> allNodes; // 메모리 해제용

    while (!openList.empty()) {
        Node* current = openList.top();
        openList.pop();

        // 경로를 찾았다!
        if (current->x == goal.first && current->y == goal.second) {
            // 경로 복원
            std::vector<std::pair<int, int>> path;
            while (current) {
                path.emplace_back(current->x, current->y);
                current = current->parent;
            }
            std::reverse(path.begin(), path.end());
            for (auto node : allNodes) delete node;
            return path;
        }

        closedSet.insert(nodeIndex(current->x, current->y));

        for (auto [nx, ny] : GetNeighbors(current->x, current->y, width, height)) {
            if (map[ny][nx] == -1) continue; // 장애물
            if (closedSet.count(nodeIndex(nx, ny))) continue;
            
            // 직선: 1.0, 대각선: 1.4
            float moveCost = (std::abs(nx - current->x) + std::abs(ny - current->y) == 2) ? 1.4f : 1.0f;
            float gNew = current->gCost + moveCost;
            float hNew = Heuristic(nx, ny, goal.first, goal.second);

            Node* neighbor = new Node(nx, ny, gNew, hNew, current);
            openList.push(neighbor);
            allNodes.push_back(neighbor);
        }
    }
    // 여기로 나왔으면 경로를 찾지 못한 경우
    for (auto node : allNodes) delete node;
    return {}; // 경로 없음
}