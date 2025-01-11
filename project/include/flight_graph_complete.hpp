#pragma once
#include "abstract_flight_graph.hpp"
#include "flight_database.hpp"

class FlightGraphComplete : public AbstractFlightGraph {
   private:
    std::shared_ptr<FlightDatabase> flight_database;

   public:
    FlightGraphComplete(std::shared_ptr<FlightDatabase> flight_database);

   protected:
    virtual int Weight(FlightDatabase::Record& record) const { return 0; }
    virtual std::shared_ptr<Vector<EdgeKey>> DiscreteChildrenOf(NodeKey node) const override;
    virtual bool IsContinuousChild(NodeKey parent, NodeKey child) const override;
};
