#pragma once
#include "flight_graph_airport_only.hpp"
#include "flight_graph_complete.hpp"

class Planner {
   private:
    std::shared_ptr<FlightDatabase> db;

   public:
    Planner(std::shared_ptr<FlightDatabase> db)
        : db(db) {}

    using PNode = std::shared_ptr<AbstractFlightGraph::Node>;

    std::shared_ptr<List<Airport>> EnumerateAirportsDFS(Airport airport, DateTime datetime_from) {
        auto graph = std::make_shared<FlightGraphAirportOnly>(db);
        auto result = std::make_shared<List<Airport>>();
        graph->OnNodeDiscovered.AddListener([&](auto node) {
            result->push_back(node->Key().airport);
        });
        auto node = graph->GetNode({airport, datetime_from});
        node->DFS();
        return result;
    }

    std::shared_ptr<List<Airport>> EnumerateAirportsBFS(Airport airport, DateTime datetime_from) {
        auto graph = std::make_shared<FlightGraphAirportOnly>(db);
        auto result = std::make_shared<List<Airport>>();
        graph->OnNodeDiscovered.AddListener([&](auto node) {
            result->push_back(node->Key().airport);
        });
        auto node = graph->GetNode({airport, datetime_from});
        node->BFS();
        return result;
    }

    using Path = std::shared_ptr<Vector<FlightDatabase::Record>>;
    using PathList = std::shared_ptr<List<Path>>;

    PathList EnumerateAllPaths(
        int airport_from,
        int airport_to,
        DateTime datetime_from = LONG_LONG_MIN,
        DateTime datetime_to = LONG_LONG_MAX,
        int depth_limit = 2) {
        auto graph = std::make_shared<FlightGraphComplete>(db);
        auto from = graph->GetNode({airport_from, datetime_from});
        auto to = graph->GetNode({airport_to, datetime_to});
        auto paths = graph->AllPathsTo(from, to, depth_limit);
        return ConvertPathList(paths);
    }

    struct ShortestPathResult {
    };
    ShortestPathResult query_shortest_path(int airport_1, int airport_2, int start_time, int end_time);

    struct MinimumCostPathResult {
    };
    MinimumCostPathResult query_minimum_cost_path(int airport_1, int airport_2, int start_time, int end_time);

   private:
    Path ConvertPath(AbstractFlightGraph::Path path) {
        auto result = std::make_shared<Vector<FlightDatabase::Record>>();
        for (auto to = path->begin(), from = to++; to != path->end(); from = to++) {
            auto record = db->QueryRecordByAirportsAndArrivalTime(
                (*from)->Key().airport, (*to)->Key().airport, (*to)->Key().no_sooner_than);
            result->push_back(record);
        }
        return result;
    }

    PathList ConvertPathList(AbstractFlightGraph::PathList path_list) {
        auto result = std::make_shared<List<Path>>();
        for (auto path : *path_list)
            result->push_back(ConvertPath(path));
        return result;
    }
};
