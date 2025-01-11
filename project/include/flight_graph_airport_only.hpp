#pragma once
#include "abstract_flight_graph.hpp"
#include "flight_database.hpp"

class FlightGraphAirportOnly : public AbstractFlightGraph {
   private:
    std::shared_ptr<FlightDatabase> flight_database;

   public:
    FlightGraphAirportOnly(std::shared_ptr<FlightDatabase> flight_database);

   protected:
    virtual std::shared_ptr<Vector<EdgeKey>> DiscreteChildrenOf(NodeKey node) const override;
    virtual bool IsContinuousChild(NodeKey parent, NodeKey child) const override;
};
