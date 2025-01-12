#include "../include/flight_graph_airport_only.hpp"
#include <algorithm>

FlightGraphAirportOnly::FlightGraphAirportOnly(std::shared_ptr<FlightDatabase> flight_database)
    : AbstractFlightGraph(flight_database->AirportRange()), flight_database(flight_database) {}

std::shared_ptr<Vector<FlightGraphAirportOnly::EdgeKey>>
FlightGraphAirportOnly::DiscreteChildrenOf(NodeKey node) const {
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

bool FlightGraphAirportOnly::IsContinuousChild(NodeKey parent, NodeKey child) const {
    return false;
}