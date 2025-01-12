#pragma once
#include <optional>
#include "abstract_partial_ordering_graph_node.hpp"

template <typename NodeKey, typename ConcreteGraph>
class AbstractGraph {
   public:
    class Node : public AbstractPartialOrderingGraphNode<Node> {
        friend class AbstractGraph<NodeKey, ConcreteGraph>;

       private:
        using Edge = AbstractPartialOrderingGraphNode<Node>::Edge;
        using PNode = AbstractPartialOrderingGraphNode<Node>::PNode;
        std::weak_ptr<AbstractGraph<NodeKey, ConcreteGraph>> graph;
        const NodeKey key;
        std::shared_ptr<Vector<Edge>> discrete_children;

        Node(std::weak_ptr<AbstractGraph<NodeKey, ConcreteGraph>> graph, NodeKey key)
            : graph(graph), key(key) {}
        void LoadDiscreteChildren() {
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

       protected:
        std::shared_ptr<Vector<Edge>> DiscreteChildren() override {
            if (!discrete_children)
                LoadDiscreteChildren();
            return discrete_children;
        }

        bool IsContinuousChild(PNode child) override {
            auto graph = this->graph.lock();
            return graph->IsContinuousChild(key, child->Key());
        }

       public:
        NodeKey Key() const { return key; }
    };

   protected:
    using PNode = std::shared_ptr<Node>;
    struct EdgeKey {
        NodeKey node;
        int weight;
        bool operator==(const EdgeKey& other) const {
            return node == other.node && weight == other.weight;
        }
    };

    virtual std::shared_ptr<Vector<EdgeKey>> DiscreteChildrenOf(NodeKey node) const = 0;
    virtual bool IsContinuousChild(NodeKey parent, NodeKey child) const = 0;
    virtual void AddNodeToPool(NodeKey node, PNode node_ptr) = 0;
    virtual std::optional<PNode> GetNodeFromPool(NodeKey node) = 0;

   public:
    virtual ~AbstractGraph() = default;

    mutable SurakartaEvent<PNode> OnNodeDiscovered;
    mutable SurakartaEvent<PNode> OnNodeVisited;

    PNode GetNode(NodeKey key) {
        auto node_opt = GetNodeFromPool(key);
        if (node_opt)
            return *node_opt;
        auto node = std::shared_ptr<Node>(new Node(static_cast<ConcreteGraph*>(this)->shared_from_this(), key));
        node->OnDiscovered.AddListener([this, node]() { OnNodeDiscovered.Invoke(node); });
        node->OnVisited.AddListener([this, node]() { OnNodeVisited.Invoke(node); });
        AddNodeToPool(key, node);
        return node;
    }

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

    std::optional<Path> BestPathTo(PNode from, PNode to) {
        OnNodeVisited.AddListener([to](auto node) {
            if (node->IsContinuousChild(to)) {
                throw node->GetPath();
            }
        });
        try {
            from->PFS();
        } catch (Path path) {
            return path;
        }
        return std::nullopt;
    }
};
