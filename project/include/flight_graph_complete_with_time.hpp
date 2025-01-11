#pragma once
#include "flight_graph_complete.hpp"

class FlightGraphCompleteWithTime : public FlightGraphComplete {
   public:
    FlightGraphCompleteWithTime(std::shared_ptr<FlightDatabase> flight_database)
        : FlightGraphComplete(flight_database) {};

   protected:
    virtual int Weight(NodeKey from, FlightDatabase::Record& record) const override {
        return record.datetime_to - from.no_sooner_than;
    }
};
