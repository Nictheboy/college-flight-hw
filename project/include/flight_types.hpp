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
            throw std::out_of_range("Airport out of range");
    }
};
