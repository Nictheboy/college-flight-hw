#pragma once
#include <climits>
#include <functional>
#include <memory>
#include <miniSTL/stl.hpp>
#include <queue>
#include "surakarta_event.hpp"

template <typename ConcreteNode>
class AbstractNode
    : public std::enable_shared_from_this<ConcreteNode> {
   public:
    AbstractNode() {}
    virtual ~AbstractNode() = default;

    using Node = ConcreteNode;
    using PNode = std::shared_ptr<Node>;
    using Path = std::shared_ptr<List<PNode>>;

    // If a node is undiscovered, then all of its children are undiscovered.
    enum class Status {
        UNDISCOVERED,
        DISCOVERED,
        VISITED
    };

   protected:
    struct Edge {
        PNode node;
        int weight;
    };

    virtual std::shared_ptr<Vector<Edge>> DiscreteChildren() = 0;
    virtual bool IsContinuousChild(PNode child) = 0;

   private:
    Status status = Status::UNDISCOVERED;
    int priority = INT_MAX;

    PNode parent;

   public:
    Status GetStatus() { return status; }
    SurakartaEvent<> OnDiscovered;
    SurakartaEvent<> OnVisited;
    void Discover(bool emit_event = true) {
        assert(status == Status::UNDISCOVERED);
        status = Status::DISCOVERED;
        if (emit_event)
            OnDiscovered.Invoke();
    }
    void Visit(bool emit_event = true) {
        assert(status == Status::DISCOVERED);
        status = Status::VISITED;
        if (emit_event)
            OnVisited.Invoke();
    }
    Path GetPath() {
        auto path = std::make_shared<List<PNode>>();
        auto node = static_cast<Node*>(this);
        while (node != nullptr) {
            path->push_front(node->shared_from_this());
            node = static_cast<Node*>(node->parent.get());
        }
        return path;
    }

    void DFS(int depth_limit = INT_MAX) {
        Discover();
        auto children = DiscreteChildren();
        if (depth_limit > 0)
            for (auto& child : *children)
                if (child.node->status == Status::UNDISCOVERED)
                    child.node->DFS(depth_limit - 1);
        Visit();
    }

    void BFS() {
        auto update_priority = [](Node* parent, Node* child, int weight) {
            child->priority = parent->priority + 1;
        };
        PFS(update_priority);
    }

    void PFS() {
        auto update_priority = [](Node* parent, Node* child, int weight) {
            if (parent->priority + weight < child->priority) {
                child->priority = parent->priority + weight;
                child->parent = parent->shared_from_this();
            }
        };
        PFS(update_priority);
    }

    using PriorityUpdater = std::function<void(Node* parent, Node* child, int weight)>;
    void PFS(PriorityUpdater update_priority) {
        auto queue = std::priority_queue<Node*, Vector<Node*>, std::function<bool(Node*, Node*)>>(
            [](Node* a, Node* b) { return a->priority > b->priority; });
        priority = 0;
        Discover();
        queue.push(static_cast<Node*>(this));
        while (!queue.empty()) {
            auto node = queue.top();
            queue.pop();
            assert(node->status != Status::UNDISCOVERED);
            if (node->status == Status::DISCOVERED) {
                auto edges = ((AbstractNode*)node)->DiscreteChildren();
                for (auto& edge : *edges) {
                    auto new_node = edge.node.get();
                    if (new_node->status == Status::UNDISCOVERED) {
                        update_priority(node, new_node, edge.weight);
                        new_node->Discover();
                        queue.push(new_node);
                    }
                }
                node->Visit();
            }
        }
    }
};
