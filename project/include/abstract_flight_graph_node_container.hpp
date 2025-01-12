#pragma once
#include <assert.h>
#include <algorithm>
#include <functional>
#include <miniSTL/stl.hpp>
#include <optional>
#include "flight_types.hpp"

template <typename T>
class AbstractFlightGraphNodeContainer {
   private:
    Vector<std::shared_ptr<Vector<T>>> elements;
    Vector<std::shared_ptr<Vector<DateTime>>> datetimes;
    AirportRange airport_range;

   public:
    AbstractFlightGraphNodeContainer(AirportRange airport_range);
    void Add(Airport airport, DateTime datetime, T element);
    std::optional<T> Get(Airport airport, DateTime datetime) const;
    std::shared_ptr<Vector<T>> Get(Airport airport) const;
    void Sort(std::function<bool(T, T)> compare);
};

template <typename T>
inline AbstractFlightGraphNodeContainer<T>::AbstractFlightGraphNodeContainer(AirportRange airport_range)
    : airport_range(airport_range) {
    auto size = airport_range.max - airport_range.min + 1;
    elements.resize(size);
    datetimes.resize(size);
    for (int i = 0; i < size; i++) {
        elements[i] = std::make_shared<Vector<T>>();
        datetimes[i] = std::make_shared<Vector<DateTime>>();
    }
}

template <typename T>
inline void AbstractFlightGraphNodeContainer<T>::Add(Airport airport, DateTime datetime, T element) {
    airport_range.WithinOrThrow(airport);
    elements[airport - airport_range.min]->push_back(element);
    datetimes[airport - airport_range.min]->push_back(datetime);
}

template <typename T>
inline std::optional<T> AbstractFlightGraphNodeContainer<T>::Get(Airport airport, DateTime datetime) const {
    airport_range.WithinOrThrow(airport);
    auto dt_list = datetimes[airport - airport_range.min];
    auto size = dt_list->size();
    auto index = -1;
    for (auto i = 0; i < size; i++)
        if ((*dt_list)[i] == datetime) {
            if (index != -1)
                throw std::runtime_error("Non-unique (Airport, DateTime)");
            index = i;
        }
    if (index >= 0)
        return (*elements[airport - airport_range.min])[index];
    else
        return std::nullopt;
}

template <typename T>
inline std::shared_ptr<Vector<T>> AbstractFlightGraphNodeContainer<T>::Get(Airport airport) const {
    airport_range.WithinOrThrow(airport);
    return elements[airport - airport_range.min];
}

template <typename T>
inline void AbstractFlightGraphNodeContainer<T>::Sort(std::function<bool(T, T)> compare) {
    auto total_size = elements.size();
    auto compare_pair = [compare](const std::pair<DateTime, T>& a, const std::pair<DateTime, T>& b) {
        return compare(a.second, b.second);
    };
    for (int i = 0; i < total_size; i++) {
        auto merged = std::make_shared<Vector<std::pair<DateTime, T>>>();
        auto size = elements[i]->size();
        assert(size == datetimes[i]->size());
        for (int j = 0; j < size; j++)
            merged->push_back({(*datetimes[i])[j], (*elements[i])[j]});
        std::sort(merged->begin(), merged->end(), compare_pair);
        for (int j = 0; j < size; j++) {
            (*datetimes[i])[j] = (*merged)[j].first;
            (*elements[i])[j] = (*merged)[j].second;
        }
    }
}
