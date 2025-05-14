#pragma once
#include <memory>

class Engine;
class GraphNode;
class GraphEdge;

class Graph{
private:
    Engine* engine;
    std::vector<std::shared_ptr<GraphNode>> nodes;
    std::vector<std::shared_ptr<GraphEdge>> edges;

public:
    Graph(Engine* engine);

    void AddNode(std::shared_ptr<GraphNode> node);
    void AddEdge(std::shared_ptr<GraphEdge> edge);
    void DeleteEdge(std::shared_ptr<GraphEdge> edge);

    std::vector<std::shared_ptr<GraphNode>> GetTerminalNodesInPath(std::shared_ptr<GraphNode> srcNode, const Event& event);
    void BroadcastEvent(const Event& event);
    void PrintOut() const;
};

class GraphEdge{
private:
        std::shared_ptr<GraphNode> srcNode;
        std::shared_ptr<GraphNode> tarNode;

public:
        GraphEdge(std::shared_ptr<GraphNode> src, std::shared_ptr<GraphNode> tar);
        std::shared_ptr<GraphNode> getSrcNode() const;
        std::shared_ptr<GraphNode> getTarNode() const;
};

class GraphNode {
private:
        std::string port;
        Model* model;
public:
        GraphNode(void* model, const std::string& modelID, const std::string& port);

        const std::string& getModelID() const;
        const std::string& getPort() const;
        void* getModel() const;

        std::string toString() const;
        bool equals(const GraphNode& other) const;
};