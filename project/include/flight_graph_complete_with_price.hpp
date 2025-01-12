#pragma once
#include "flight_graph_complete.hpp"

class FlightGraphCompleteWithPrice : public FlightGraphComplete {
   public:
    FlightGraphCompleteWithPrice(std::shared_ptr<FlightDatabase> flight_database)
        : FlightGraphComplete(flight_database) {};

   protected:
    virtual int Weight(FlightNodeKey from, FlightDatabase::Record& record) const override {
        return record.price;
    }
};
