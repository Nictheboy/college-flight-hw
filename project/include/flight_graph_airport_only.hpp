#pragma once
#include "abstract_flight_graph.hpp"
#include "flight_database.hpp"

class FlightGraphAirportOnly : public AbstractFlightGraph {
   private:
    std::shared_ptr<FlightDatabase> flight_database;

   public:
    FlightGraphAirportOnly(std::shared_ptr<FlightDatabase> flight_database)
        : AbstractFlightGraph(flight_database->AirportRange()), flight_database(flight_database) {}

   protected:
    virtual std::shared_ptr<Vector<EdgeKey>> DiscreteChildrenOf(FlightNodeKey node) const override {
        auto airport = node.airport;
        auto no_sooner_than = node.no_sooner_than;
        auto record_ids = flight_database->QueryRecordIdsByAirportFrom(airport);
        auto edge_keys = std::make_shared<Vector<EdgeKey>>();
        edge_keys->reserve(record_ids->size());
        for (auto record_id : *record_ids) {
            auto record = flight_database->QueryRecordById(record_id);
            edge_keys->push_back({{record.airport_to, 0}, 0});
        }
        auto last = std::unique(edge_keys->begin(), edge_keys->end());
        edge_keys->erase(last, edge_keys->end());
        return edge_keys;
    }

    virtual bool IsContinuousChild(FlightNodeKey parent, FlightNodeKey child) const override {
        return false;
    }
};
