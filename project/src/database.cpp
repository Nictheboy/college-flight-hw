#include "database.hpp"

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

std::shared_ptr<FlightDatabase::Node> FlightDatabase::GetNode(Airport airport, DateTime no_sooner_than) {
    auto nodes = node_pool[airport - airport_min];
    for (auto& node : *nodes) {
        if (node->NoSoonerThan() == no_sooner_than)
            return node;
    }
    auto node = std::make_shared<Node>(*this, airport, no_sooner_than);
    nodes->push_back(node);
    return node;
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
    InitNodePool();
    logger->Log("");
    logger->Log("Bootstrap complete. See README.md for usage.");
}

FlightDatabase::DateTime FlightDatabase::ParseDateTime(std::string datetime) {
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

void FlightDatabase::Node::LoadDiscreteChildren() {
    assert(!discrete_children);
    auto ids = db.QueryRecordIdsByAirportFrom(airport);
    discrete_children = std::make_shared<Vector<Edge>>();
    discrete_children->reserve(ids->size());
    for (auto id : *ids) {
        auto record = db.QueryRecordById(id);
        if (record.datetime_from >= no_sooner_than) {
            auto node = db.GetNode(record.airport_to, record.datetime_to);
            discrete_children->push_back({node, record.price});
        }
    }
}
