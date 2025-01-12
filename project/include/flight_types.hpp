#pragma once
#include <stdexcept>

using Key = int;
using Airport = int;
using DateTime = long long;
using Price = int;

struct AirportRange {
    Airport min, max;

    bool Within(Airport airport) const {
        return airport >= min && airport <= max;
    }

    void WithinOrThrow(Airport airport) const {
        if (!Within(airport))
            throw std::out_of_range("Airport " + std::to_string(airport) + " out of range");
    }
};

struct FlightNodeKey {
    Airport airport;
    DateTime no_sooner_than;
    bool operator==(const FlightNodeKey& other) const {
        return airport == other.airport && no_sooner_than == other.no_sooner_than;
    }
};
