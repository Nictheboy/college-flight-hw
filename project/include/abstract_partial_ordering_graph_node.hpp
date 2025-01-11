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
    using PAbstractNode = std::shared_ptr<AbstractNode>;
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
    Status status = Status::UNDISCOVERED;
    int priority;

    SurakartaEvent<Node&> OnDiscovered;
    SurakartaEvent<Node&> OnVisited;

    std::shared_ptr<Vector<Edge>> DFS() {
        ResetSubgraphStatus();
        auto list = std::make_shared<Vector<Edge>>();
        RecursiveDFS(list);
        return list;
    }

    std::shared_ptr<Vector<Edge>> BFS() {
        auto update_priority = [](AbstractNode& node, Edge& edge) {
            edge.node->priority = node.priority + 1;
        };
        return PFS(update_priority);
    }

    std::shared_ptr<Vector<Edge>> PFS(std::function<void(AbstractNode&, Edge&)> update_priority) {
        ResetSubgraphStatus();
        auto list = std::make_shared<Vector<Edge>>();
        auto queue = std::priority_queue<Edge, Vector<Edge>, std::function<bool(Edge, Edge)>>(
            [](Edge a, Edge b) { return a.node->priority > b.node->priority; });
        auto children = DiscreteChildren();
        priority = 0;
        for (auto& child : *children) {
            update_priority(*this, child);
            child.node->status = Status::DISCOVERED;
            OnDiscovered.Invoke(*child.node);
            queue.push(child);
        }
        while (!queue.empty()) {
            auto edge = queue.top();
            queue.pop();
            assert(edge.node->status != Status::UNDISCOVERED);
            if (edge.node->status == Status::DISCOVERED) {
                list->push_back(edge);
                edge.node->status = Status::VISITED;
                OnVisited.Invoke(*edge.node);
                auto children = static_cast<PAbstractNode>(edge.node)->DiscreteChildren();
                for (auto& child : *children) {
                    if (child.node->status == Status::UNDISCOVERED) {
                        update_priority(*edge.node, child);
                        child.node->status = Status::DISCOVERED;
                        OnDiscovered.Invoke(*child.node);
                        queue.push(child);
                    }
                }
            }
        }
        return list;
    }

   private:
    void ResetSubgraphStatus() {
        if (status != Status::UNDISCOVERED) {
            auto children = DiscreteChildren();
            for (auto& child : *children) {
                child.node->ResetSubgraphStatus();
            }
            status = Status::UNDISCOVERED;
        }
    }

    void RecursiveDFS(std::shared_ptr<Vector<Edge>> list) {
        assert(status == Status::UNDISCOVERED);
        status = Status::DISCOVERED;
        OnDiscovered.Invoke(*static_cast<Node*>(this));
        auto children = DiscreteChildren();
        for (auto& child : *children) {
            if (child.node->status == Status::UNDISCOVERED) {
                list->push_back(child);
                child.node->RecursiveDFS(list);
            }
        }
        status = Status::VISITED;
        OnVisited.Invoke(*static_cast<Node*>(this));
    }
};
