#include "../include/flight_database.hpp"
#include <assert.h>
#include <algorithm>
#include <fstream>

void FlightDatabase::LoadDatabase(std::string filename) {
    auto file = std::ifstream();
    file.open(filename);
    if (!file.is_open())
        throw std::runtime_error("Failed to open file: " + filename);
    auto line = std::string();
    std::getline(file, line);
    while (true) {
        std::getline(file, line);
        if (line.empty())
            break;
        records.push_back(ParseRecord(line));
    }
}

FlightDatabase::Record FlightDatabase::ParseRecord(std::string line) {
    // Flight ID,Departure date,Intl/Dome,Flight NO.,Departure airport,Arrival airport,Departure Time,Arrival Time,Airplane ID,Airplane Model,Air fares
    // 1,5/5/2017,Dome,346,48,50,5/5/2017 12:20,5/5/2017 15:10,30,1,666
    auto record = Record();
    record.id = std::stoi(line.substr(0, line.find(',')));
    line = line.substr(line.find(',') + 1);
    line = line.substr(line.find(',') + 1);
    line = line.substr(line.find(',') + 1);
    line = line.substr(line.find(',') + 1);
    record.airport_from = std::stoi(line.substr(0, line.find(',')));
    line = line.substr(line.find(',') + 1);
    record.airport_to = std::stoi(line.substr(0, line.find(',')));
    line = line.substr(line.find(',') + 1);
    record.datetime_from = ParseDateTime(line.substr(0, line.find(',')));
    line = line.substr(line.find(',') + 1);
    record.datetime_to = ParseDateTime(line.substr(0, line.find(',')));
    line = line.substr(line.find(',') + 1);
    line = line.substr(line.find(',') + 1);
    line = line.substr(line.find(',') + 1);
    record.price = std::stoi(line);
    return record;
}

FlightDatabase::FlightDatabase(std::string filename) {
    LoadDatabase(filename);
    InitAirportRange();
    InitAirportBucketIndex();
}

DateTime FlightDatabase::ParseDateTime(std::string datetime) {
    // 5/6/2017 12:20
    DateTime month = std::stoi(datetime.substr(0, datetime.find('/')));
    datetime = datetime.substr(datetime.find('/') + 1);
    DateTime day = std::stoi(datetime.substr(0, datetime.find('/')));
    datetime = datetime.substr(datetime.find('/') + 1);
    DateTime year = std::stoi(datetime.substr(0, datetime.find(' ')));
    datetime = datetime.substr(datetime.find(' ') + 1);
    DateTime hour = std::stoi(datetime.substr(0, datetime.find(':')));
    datetime = datetime.substr(datetime.find(':') + 1);
    DateTime minute = std::stoi(datetime);
    return (year * 10000 + month * 100 + day) * 10000 + hour * 100 + minute;
}

FlightDatabase::Record FlightDatabase::QueryRecordById(Key id) const {
    return records[id - 1];
}

FlightDatabase::Record
FlightDatabase::QueryRecordByAirportsAndArrivalTime(
    Airport airport_from,
    Airport airport_to,
    DateTime datetime_to) const {
    auto subquery = QueryRecordIdsByAirportFrom(airport_from);
    auto result = std::optional<Record>();
    for (auto id : *subquery) {
        auto record = QueryRecordById(id);
        if (record.airport_to == airport_to && record.datetime_to == datetime_to) {
            if (result.has_value())
                throw std::runtime_error("Unique violation!");
            result = record;
        }
    }
    if (!result.has_value())
        throw std::runtime_error("Record not found!");
    return result.value();
}

std::shared_ptr<Vector<Key>>
FlightDatabase::QueryRecordIdsByAirportFrom(Airport airport) const {
    return airport_from_bucket_index->Get(airport);
}

std::shared_ptr<Vector<Key>>
FlightDatabase::QueryRecordIdsByAirportTo(Airport airport) const {
    return airport_to_bucket_index->Get(airport);
}

void FlightDatabase::InitAirportRange() {
    airport_range.min = airport_range.max = records[0].airport_from;
    for (auto record : records) {
        airport_range.min = std::min(airport_range.min, record.airport_from);
        airport_range.max = std::max(airport_range.max, record.airport_from);
        airport_range.min = std::min(airport_range.min, record.airport_to);
        airport_range.max = std::max(airport_range.max, record.airport_to);
    }
}

void FlightDatabase::InitAirportBucketIndex() {
    airport_from_bucket_index =
        std::make_shared<AbstractFlightGraphNodeContainer<Key>>(airport_range);
    airport_to_bucket_index =
        std::make_shared<AbstractFlightGraphNodeContainer<Key>>(airport_range);
    for (auto& record : records) {
        airport_from_bucket_index->Add(record.airport_from, record.datetime_from, record.id);
        airport_to_bucket_index->Add(record.airport_to, record.datetime_to, record.id);
    }
    airport_from_bucket_index->Sort([this](Key a, Key b) {
        auto& record_a = records[a - 1];
        auto& record_b = records[b - 1];
        return record_a.datetime_from < record_b.datetime_from ||
               (record_a.datetime_from == record_b.datetime_from &&
                record_a.airport_to < record_b.airport_to);
    });
    airport_to_bucket_index->Sort([this](Key a, Key b) {
        auto& record_a = records[a - 1];
        auto& record_b = records[b - 1];
        return record_a.datetime_to < record_b.datetime_to ||
               (record_a.datetime_to == record_b.datetime_to &&
                record_a.airport_from < record_b.airport_from);
    });
}
