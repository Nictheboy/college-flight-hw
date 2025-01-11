#pragma once
#include <memory>
#include <miniSTL/stl.hpp>

template <typename ConcreteNode>
class AbstractPartialOrderingGraphNode {
   protected:
    using Node = ConcreteNode;
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

    std::shared_ptr<Vector<Edge>> DFS() {
        ResetSubgraphStatus();
        auto list = std::make_shared<Vector<Edge>>();
        RecursiveDFS(list);
        return list;
    }

   private:
    // If a node is undiscovered, then all of its children are undiscovered.
    enum class Status {
        UNDISCOVERED,
        DISCOVERED,
        VISITING,
        VISITED
    };
    Status status = Status::UNDISCOVERED;

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
        status = Status::VISITING;
        auto children = DiscreteChildren();
        for (auto& child : *children) {
            assert(child.node->status == Status::UNDISCOVERED || child.node->status == Status::VISITED);
            if (child.node->status == Status::UNDISCOVERED) {
                list->push_back(child);
                child.node->RecursiveDFS(list);
            }
        }
        status = Status::VISITED;
    }
};
