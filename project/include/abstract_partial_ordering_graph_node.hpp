#pragma once
#include <functional>
#include <memory>
#include <miniSTL/stl.hpp>
#include <queue>
#include "surakarta_event.hpp"

template <typename ConcreteNode>
class AbstractPartialOrderingGraphNode {
   protected:
    using Node = ConcreteNode;
    using AbstractNode = AbstractPartialOrderingGraphNode;
    using PNode = std::shared_ptr<Node>;
    struct Edge {
        PNode node;
        int weight;
    };

    virtual std::shared_ptr<Vector<Edge>> DiscreteChildren() = 0;
    virtual bool IsContinuousChild(PNode child) = 0;

   public:
    AbstractPartialOrderingGraphNode() {}
    virtual ~AbstractPartialOrderingGraphNode() = default;

    // If a node is undiscovered, then all of its children are undiscovered.
    enum class Status {
        UNDISCOVERED,
        DISCOVERED,
        VISITED
    };

   private:
    Status status = Status::UNDISCOVERED;
    int priority;

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

    void DFS() {
        ResetSubgraphStatus();
        RecursiveDFS();
    }

    using PriorityUpdater = std::function<void(Node* parent, Node* child, int weight)>;

    void BFS() {
        auto update_priority = [](Node* parent, Node* child, int weight) {
            child->priority = parent->priority + 1;
        };
        PFS(update_priority);
    }

    void PFS(PriorityUpdater update_priority) {
        ResetSubgraphStatus();
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

   private:
    void ResetSubgraphStatus() {
        if (status != Status::UNDISCOVERED) {
            auto children = DiscreteChildren();
            for (auto& child : *children)
                child.node->ResetSubgraphStatus();
            status = Status::UNDISCOVERED;
        }
    }

    void RecursiveDFS() {
        Discover();
        auto children = DiscreteChildren();
        for (auto& child : *children)
            if (child.node->status == Status::UNDISCOVERED)
                child.node->RecursiveDFS();
        Visit();
    }
};
