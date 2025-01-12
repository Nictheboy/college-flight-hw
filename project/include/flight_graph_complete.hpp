#pragma once
#include "abstract_flight_graph.hpp"
#include "flight_database.hpp"

class FlightGraphComplete : public AbstractFlightGraph {
   private:
    std::shared_ptr<FlightDatabase> flight_database;

   public:
    FlightGraphComplete(std::shared_ptr<FlightDatabase> flight_database);

   protected:
    virtual int Weight(FlightNodeKey from, FlightDatabase::Record& record) const { return 0; }
    virtual std::shared_ptr<Vector<EdgeKey>> DiscreteChildrenOf(FlightNodeKey node) const override;
    virtual bool IsContinuousChild(FlightNodeKey parent, FlightNodeKey child) const override;
};
