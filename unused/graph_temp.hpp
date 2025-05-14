#pragma once
#include <vector>
#include <unordered_map>
#include <string>
#include <memory>
#include <algorithm>
#include <iostream>

class GraphNode;
class GraphEdge;
class Event;
class Engine;
class Model;

// Graph
class Graph{
private:
	Engine* engine;
	std::vector<std::shared_ptr<GraphNode>> nodes;
	std::vector<std::shared_ptr<GraphEdge>> edges;
	std::unordered_map<std::string, std::vector<std::string>> adjacentNodes;
	std::unordered_map<std::string, std::shared_ptr<GraphNode>> nodesWithID;

public:
	Graph(Engine* engine);

	void addNode(std::shared_ptr<GraphNode> node);
	void addEdge(std::shared_ptr<GraphEdge> edge);
	void deleteEdge(std::shared_ptr<GraphEdge> edge);

	std::vector<std::shared_ptr<GraphNode>> getTerminalNodesInPath(std::shared_ptr<GraphNode> srcNode, const Event& event);
	void broadcastEvent(const Event& event);
	void printOut() const;
};

// GraphEdge
class GraphEdge{
	private:
			std::shared_ptr<GraphNode> srcNode;
			std::shared_ptr<GraphNode> tarNode;
	
	public:
			GraphEdge(std::shared_ptr<GraphNode> src, std::shared_ptr<GraphNode> tar);
			std::shared_ptr<GraphNode> getSrcNode() const;
			std::shared_ptr<GraphNode> getTarNode() const;
};

// GraphNode
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