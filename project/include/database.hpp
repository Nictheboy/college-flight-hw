#pragma once
#include <assert.h>
#include <fstream>
#include <graph.hpp>
#include <memory>
#include <miniSTL/stl.hpp>
#include <optional>
#include <string>
#include "surakarta_logger.hpp"

class FlightDatabase {
   public:
    FlightDatabase(std::string filename, std::shared_ptr<SurakartaLogger> logger = std::make_shared<SurakartaLoggerNull>());

    using Key = int;
    using Airport = int;
    using DateTime = long long;
    using Price = int;
    struct Record {
        Key id;
        Airport airport_from, airport_to;
        DateTime datetime_from, datetime_to;
        Price price;
    };
    class Node : public PartialOrderingGraphNode<Node> {
       private:
        FlightDatabase& db;
        Airport airport;
        DateTime no_sooner_than;
        std::shared_ptr<Vector<Edge>> discrete_children;
        void LoadDiscreteChildren();

       public:
        Node(FlightDatabase& db, Airport airport, DateTime no_sooner_than)
            : db(db), airport(airport), no_sooner_than(no_sooner_than) {}
        Airport Airport() const { return airport; }
        DateTime NoSoonerThan() const { return no_sooner_than; }
        std::shared_ptr<Vector<Edge>> DiscreteChildren() override {
            if (!discrete_children)
                LoadDiscreteChildren();
            return discrete_children;
        }
        bool IsContinuousChildren(PNode child) override {
            return child->airport == airport && child->no_sooner_than >= no_sooner_than;
        }
        bool operator==(const Node& other) const {
            return airport == other.airport && no_sooner_than == other.no_sooner_than;
        }
    };

    DateTime ParseDateTime(std::string datetime);
    std::shared_ptr<Node> GetNode(Airport airport, DateTime no_sooner_than);

   private:
    std::shared_ptr<SurakartaLogger> logger;

    Vector<Record> records;
    Record ParseRecord(std::string line);
    void LoadDatabase(std::string filename);

    Airport airport_min, airport_max;
    void InitAirportRange();

    Vector<std::shared_ptr<List<Key>>> airport_from_bucket_index, airport_to_bucket_index;
    void InitAirportBucketIndex();

    Record QueryRecordById(Key id) const { return records[id - 1]; }
    std::shared_ptr<List<Key>> QueryRecordIdsByAirportFrom(Airport airport) const {
        return airport_from_bucket_index[airport - airport_min];
    }
    std::shared_ptr<List<Key>> QueryRecordIdsByAirportTo(Airport airport) const {
        return airport_to_bucket_index[airport - airport_min];
    }

    Vector<std::shared_ptr<List<std::shared_ptr<Node>>>> node_pool;
    void InitNodePool() {
        node_pool.resize(airport_max - airport_min + 1);
        for (Airport airport = airport_min; airport <= airport_max; ++airport) {
            node_pool[airport - airport_min] = std::make_shared<List<std::shared_ptr<Node>>>();
        }
    }
};
