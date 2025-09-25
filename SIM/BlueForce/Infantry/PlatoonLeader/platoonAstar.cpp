#include "PlatoonAStar.hpp"


static inline bool Passable(const Point& p){
    if (!ENV_REF.InBounds(p)) return false;
    return ENV_REF.GetTerrainAt(p) != TerrainType::RIVER;
}


static inline int H(const Point& a, const Point& b){
    return 10 * (std::abs(a.x - b.x) + std::abs(a.y - b.y));
}

std::vector<Point> AStarPlan(Point start, Point goal, int max_expand){
    std::vector<Point> empty;

    if (!ENV_REF.InBounds(start) || !ENV_REF.InBounds(goal)) return empty;
    if (!Passable(start)) return empty;
    if (!Passable(goal))  return empty;

    struct Node { Point p; int f, g; };
    struct Cmp  { bool operator()(const Node& a, const Node& b) const { return a.f > b.f; } };

    static const int DX[4] = {+1,-1, 0, 0};
    static const int DY[4] = { 0, 0,+1,-1};

    std::priority_queue<Node, std::vector<Node>, Cmp> open;
    std::unordered_map<Point,int,PointHash> gScore;
    std::unordered_map<Point,Point,PointHash> parent;

    gScore[start] = 0;
    open.push(Node{start, H(start,goal), 0});

    int expanded = 0;
    while (!open.empty()){
        Node cur = open.top(); open.pop();

        if (cur.p == goal){

            std::vector<Point> path;
            Point t = goal;
            while (t != start){
                path.push_back(t);
                t = parent[t];
            }
            std::reverse(path.begin(), path.end());
            return path;
        }

        if (++expanded > max_expand) break;

        for (int k=0;k<4;++k){
            Point np{cur.p.x + DX[k], cur.p.y + DY[k]};
            if (!ENV_REF.InBounds(np)) continue;
            if (!Passable(np))         continue;


            int tentative = cur.g + 10;

            auto it = gScore.find(np);
            if (it == gScore.end() || tentative < it->second){
                gScore[np] = tentative;
                parent[np] = cur.p;
                int f = tentative + H(np, goal);
                open.push(Node{np, f, tentative});
            }
        }
    }


    return empty;
}
