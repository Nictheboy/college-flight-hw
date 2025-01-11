#include "flight_database.hpp"
#include <assert.h>
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

FlightDatabase::FlightDatabase(std::string filename, std::shared_ptr<SurakartaLogger> logger)
    : logger(logger) {
    logger->Log("Loading database from %s", filename.c_str());
    LoadDatabase(filename);
    logger->Log("Creating indexes and the graph");
    InitAirportRange();
    InitAirportBucketIndex();
    logger->Log("");
    logger->Log("Bootstrap complete. See README.md for usage.");
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

std::shared_ptr<List<Key>> FlightDatabase::QueryRecordIdsByAirportFrom(Airport airport) const {
    return airport_from_bucket_index[airport - airport_min];
}

std::shared_ptr<List<Key>> FlightDatabase::QueryRecordIdsByAirportTo(Airport airport) const {
    return airport_to_bucket_index[airport - airport_min];
}

void FlightDatabase::InitAirportRange() {
    airport_min = airport_max = records[0].airport_from;
    for (auto record : records) {
        airport_min = std::min(airport_min, record.airport_from);
        airport_max = std::max(airport_max, record.airport_from);
        airport_min = std::min(airport_min, record.airport_to);
        airport_max = std::max(airport_max, record.airport_to);
    }
}

void FlightDatabase::InitAirportBucketIndex() {
    auto size = airport_max - airport_min + 1;
    airport_from_bucket_index.resize(size);
    airport_to_bucket_index.resize(size);
    for (auto i = 0; i < size; i++) {
        airport_from_bucket_index[i] = std::make_shared<List<Key>>();
        airport_to_bucket_index[i] = std::make_shared<List<Key>>();
    }
    for (auto record : records) {
        airport_from_bucket_index[record.airport_from - airport_min]->push_back(record.id);
        airport_to_bucket_index[record.airport_to - airport_min]->push_back(record.id);
    }
}
