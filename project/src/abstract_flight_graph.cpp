#include "../include/abstract_flight_graph.hpp"

void AbstractFlightGraph::Node::LoadDiscreteChildren() {
    assert(!discrete_children);
    auto graph = this->graph.lock();
    auto edge_keys = graph->DiscreteChildrenOf(key);
    discrete_children = std::make_shared<Vector<Edge>>();
    discrete_children->reserve(edge_keys->size());
    for (auto& edge_key : *edge_keys) {
        auto child = graph->GetNode(edge_key.node);
        discrete_children->push_back({child, edge_key.weight});
    }
}

std::shared_ptr<Vector<AbstractFlightGraph::Node::Edge>> AbstractFlightGraph::Node::DiscreteChildren() {
    if (!discrete_children)
        LoadDiscreteChildren();
    return discrete_children;
}

bool AbstractFlightGraph::Node::IsContinuousChild(PNode child) {
    auto graph = this->graph.lock();
    return graph->IsContinuousChild(key, child->Key());
}

AbstractFlightGraph::AbstractFlightGraph(Airport airport_min, Airport airport_max)
    : airport_min(airport_min), airport_max(airport_max) {
    node_pool.resize(airport_max - airport_min + 1);
    for (Airport airport = airport_min; airport <= airport_max; ++airport) {
        node_pool[airport - airport_min] = std::make_shared<NodesOfAirport>();
    }
}

AbstractFlightGraph::PNode AbstractFlightGraph::GetNode(NodeKey key) const {
    Airport airport = key.airport;
    DateTime no_sooner_than = key.no_sooner_than;
    auto nodes = node_pool[airport - airport_min];
    for (auto& node : *nodes) {
        if (node->key.no_sooner_than == no_sooner_than)
            return node;
    }
    auto node = std::shared_ptr<Node>(new Node(shared_from_this(), key));
    node->OnDiscovered.AddListener([this, node]() { OnNodeDiscovered.Invoke(node); });
    node->OnVisited.AddListener([this, node]() { OnNodeVisited.Invoke(node); });
    nodes->push_back(node);
    return node;
}
