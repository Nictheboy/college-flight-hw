#pragma once
#include "abstract_partial_ordering_graph_node.hpp"
#include "flight_types.hpp"

class AbstractFlightGraph : public std::enable_shared_from_this<AbstractFlightGraph> {
   protected:
    struct NodeKey {
        Airport airport;
        DateTime no_sooner_than;
        bool operator==(const NodeKey& other) const {
            return airport == other.airport && no_sooner_than == other.no_sooner_than;
        }
    };
    struct EdgeKey {
        NodeKey node;
        int weight;
        bool operator==(const EdgeKey& other) const {
            return node == other.node && weight == other.weight;
        }
    };

    virtual std::shared_ptr<Vector<EdgeKey>> DiscreteChildrenOf(NodeKey node) const = 0;
    virtual bool IsContinuousChild(NodeKey parent, NodeKey child) const = 0;

   public:
    class Node : public AbstractPartialOrderingGraphNode<Node>, std::enable_shared_from_this<Node> {
        friend class AbstractFlightGraph;

       private:
        std::weak_ptr<const AbstractFlightGraph> graph;
        const NodeKey key;
        std::shared_ptr<Vector<Edge>> discrete_children;

        Node(std::weak_ptr<const AbstractFlightGraph> graph, NodeKey key)
            : graph(graph), key(key) {}
        void LoadDiscreteChildren();

       protected:
        std::shared_ptr<Vector<Edge>> DiscreteChildren() override;
        bool IsContinuousChild(PNode child) override;

       public:
        NodeKey Key() const { return key; }
    };

   private:
    Airport airport_min, airport_max;

    using PNode = std::shared_ptr<Node>;
    using NodesOfAirport = List<PNode>;
    Vector<std::shared_ptr<NodesOfAirport>> node_pool;

   public:
    AbstractFlightGraph(Airport airport_min, Airport airport_max);
    ~AbstractFlightGraph() = default;

    mutable SurakartaEvent<PNode> OnNodeDiscovered;
    mutable SurakartaEvent<PNode> OnNodeVisited;

    PNode GetNode(NodeKey key) const;

    using Path = std::shared_ptr<List<PNode>>;
    using PathList = std::shared_ptr<List<Path>>;
    PathList AllPathsTo(PNode from, PNode to, int depth_limit) {
        auto result = std::make_shared<List<Path>>();
        auto stack = std::make_shared<List<PNode>>();
        OnNodeDiscovered.AddListener([stack](auto node) { stack->push_back(node); });
        OnNodeVisited.AddListener([stack, result, to](auto node) {
            if (node == to || node->IsContinuousChild(to)) {
                auto path = std::make_shared<List<PNode>>();
                for (auto& node : *stack)
                    path->push_back(node);
                result->push_back(path);
            }
            stack->pop_back();
        });
        from->DFS(depth_limit);
        return result;
    }
};
