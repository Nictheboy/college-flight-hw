#pragma once
#include "abstract_flight_graph_node_container.hpp"
#include "abstract_graph.hpp"
#include "flight_types.hpp"

class AbstractFlightGraph
    : public AbstractGraph<FlightNodeKey, AbstractFlightGraph>,
      public std::enable_shared_from_this<AbstractFlightGraph> {
   protected:
   private:
    const AirportRange airport_range;
    AbstractFlightGraphNodeContainer<PNode> node_pool;

   public:
    AbstractFlightGraph(AirportRange airport_range)
        : airport_range(airport_range), node_pool(airport_range) {}

    virtual void AddNodeToPool(FlightNodeKey node, PNode node_ptr) override {
        node_pool.Add(node.airport, node.no_sooner_than, node_ptr);
    }
    virtual std::optional<PNode> GetNodeFromPool(FlightNodeKey node) override {
        return node_pool.Get(node.airport, node.no_sooner_than);
    }
};
