#pragma once
#include <memory>
#include <miniSTL/stl.hpp>
#include <optional>
#include <string>
#include "abstract_flight_graph_node_container.hpp"
#include "flight_types.hpp"

class FlightDatabase {
   public:
    FlightDatabase(std::string filename);
    struct Record {
        Key id;
        Airport airport_from, airport_to;
        DateTime datetime_from, datetime_to;
        Price price;
    };

    DateTime ParseDateTime(std::string datetime);
    Record QueryRecordById(Key id) const;
    Record QueryRecordByAirportsAndArrivalTime(Airport airport_from, Airport airport_to, DateTime datetime_to) const;
    std::shared_ptr<Vector<Key>> QueryRecordIdsByAirportFrom(Airport airport) const;
    std::shared_ptr<Vector<Key>> QueryRecordIdsByAirportTo(Airport airport) const;
    ::AirportRange AirportRange() const { return airport_range; }

   private:
    Vector<Record> records;
    Record ParseRecord(std::string line);
    void LoadDatabase(std::string filename);

    ::AirportRange airport_range;
    void InitAirportRange();

    std::shared_ptr<AbstractFlightGraphNodeContainer<Key>> airport_from_bucket_index, airport_to_bucket_index;
    void InitAirportBucketIndex();
};
