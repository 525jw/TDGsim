#include "graph.hpp"
#include "event.hpp"

Graph::Graph(Engine* engine) : engine(engine){}

void Graph::addNode(std::shared_ptr<GraphNode> node){
	std::string id = node->toString();
	if (nodesWithID.find(id) == nodesWithID.end()) {
			nodes.push_back(node);
			nodesWithID[id] = node;
			adjacentNodes[id] = {};
	}
}

void Graph::addEdge(std::shared_ptr<GraphEdge> edge){
	auto src = edge->getSrcNode();
	auto tar = edge->getTarNode();
	addNode(src);
	addNode(tar);
	edges.push_back(edge);
	adjacentNodes[src->toString()].push_back(tar->toString());
}

void Graph::deleteEdge(std::shared_ptr<GraphEdge> edge){
	auto src = edge->getSrcNode();
	auto tar = edge->getTarNode();
	auto& adjList = adjacentNodes[src->toString()];
	adjList.erase(std::remove(adjList.begin(), adjList.end(), tar->toString()), adjList.end());

	edges.erase(std::remove_if(edges.begin(), edges.end(),
			[&](std::shared_ptr<GraphEdge> e) {
					return e->getSrcNode()->toString() == src->toString() &&
								 e->getTarNode()->toString() == tar->toString();
			}), edges.end());
}

std::vector<std::shared_ptr<GraphNode>> Graph::getTerminalNodesInPath(std::shared_ptr<GraphNode> srcNode, const Event& event){
	std::vector<std::shared_ptr<GraphNode>> result;
	auto& adj = adjacentNodes[srcNode->toString()];
	if (adj.empty()) {
			return { srcNode };
	}

	for (const auto& nodeID : adj) {
			if (nodesWithID.find(nodeID) != nodesWithID.end()) {
					auto node = nodesWithID[nodeID];
					if (node->isDynamicCoupledModel()) {
							// node->getModel()->funcStateTransition(node->getPort(), event.getMessage());
							// 노드의 모델을 가져와 상태 천이함수(노드의 포트, 이벤트 메시지)를 실행시킴.
					}
					auto terminalNodes = getTerminalNodesInPath(node, event);
					result.insert(result.end(), terminalNodes.begin(), terminalNodes.end());
			}
	}
	return result;
}


void Graph::broadcastEvent{
//	BroadCast Logic이 들어갈 부분
}


void Graph::printOut() const{
	std::cout << "Coupling Nodes:\n";
	for (const auto& node : nodes) {
			std::cout << node->toString() << "\n";
	}

	std::cout << "Coupling Edges:\n";
	for (const auto& edge : edges) {
			std::cout << edge->getSrcNode()->toString() << " --> " << edge->getTarNode()->toString() << "\n";
	}

	std::cout << "Adjacent Nodes:\n";
	for (const auto& [key, value] : adjacentNodes) {
			std::cout << key << " : ";
			for (const auto& v : value)
					std::cout << v << " ";
			std::cout << "\n";
	}
}

// GraphEdge
GraphEdge::GraphEdge(std::shared_ptr<GraphNode> src, std::shared_ptr<GraphNode> tar): srcNode(src), tarNode(tar) {}
std::shared_ptr<GraphNode> GraphEdge::getSrcNode() const { return srcNode; }
std::shared_ptr<GraphNode> GraphEdge::getTarNode() const { return tarNode; }

// GraphNode
GraphNode::GraphNode(void* model, const std::string& modelID, const std::string& port, bool dynamic, bool mrdevs): model(model), modelID(modelID), port(port),dynamicCoupledModel(dynamic), mrdevsCoupledModel(mrdevs) {}

const std::string& GraphNode::getModelID() const { return modelID; }
const std::string& GraphNode::getPort() const { return port; }
void* GraphNode::getModel() const { return model; }
bool GraphNode::isDynamicCoupledModel() const { return dynamicCoupledModel; }
bool GraphNode::isMRDEVSCoupledModel() const { return mrdevsCoupledModel; }

std::string GraphNode::toString() const {
    return modelID + "(" + port + ")";
}

bool GraphNode::equals(const GraphNode& other) const {
    return modelID == other.modelID && port == other.port;
}