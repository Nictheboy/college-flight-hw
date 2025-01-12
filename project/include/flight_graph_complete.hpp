#pragma once
#include "abstract_flight_graph.hpp"
#include "flight_database.hpp"

class FlightGraphComplete : public AbstractFlightGraph {
   private:
    std::shared_ptr<FlightDatabase> flight_database;

   public:
    FlightGraphComplete(std::shared_ptr<FlightDatabase> flight_database)
        : AbstractFlightGraph(flight_database->AirportRange()), flight_database(flight_database) {}

   protected:
    virtual int Weight(FlightNodeKey from, FlightDatabase::Record& record) const { return 0; }
    virtual std::shared_ptr<Vector<EdgeKey>> DiscreteChildrenOf(FlightNodeKey node) const override {
        auto airport = node.airport;
        auto no_sooner_than = node.no_sooner_than;
        auto record_ids = flight_database->QueryRecordIdsByAirportFrom(airport);
        auto edge_keys = std::make_shared<Vector<EdgeKey>>();
        edge_keys->reserve(record_ids->size());
        for (auto record_id : *record_ids) {
            auto record = flight_database->QueryRecordById(record_id);
            if (record.datetime_from < no_sooner_than)
                continue;
            edge_keys->push_back({{record.airport_to, record.datetime_to}, Weight(node, record)});
        }
        return edge_keys;
    }

    virtual bool IsContinuousChild(FlightNodeKey parent, FlightNodeKey child) const override {
        return parent.airport == child.airport && parent.no_sooner_than <= child.no_sooner_than;
    }
};
