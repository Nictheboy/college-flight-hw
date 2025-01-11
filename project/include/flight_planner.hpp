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

    std::shared_ptr<List<Airport>> EnumerateAirportsDFS(int airport_id, std::string start_time) {
        auto graph = std::make_shared<FlightGraphAirportOnly>(db);
        auto result = std::make_shared<List<Airport>>();
        graph->OnNodeDiscovered.AddListener([&](auto node) {
            result->push_back(node->Key().airport);
        });
        auto node = graph->GetNode({airport_id, db->ParseDateTime(start_time)});
        node->DFS();
        return result;
    }

    std::shared_ptr<List<Airport>> EnumerateAirportsBFS(int airport_id, std::string start_time) {
        auto graph = std::make_shared<FlightGraphAirportOnly>(db);
        auto result = std::make_shared<List<Airport>>();
        graph->OnNodeDiscovered.AddListener([&](auto node) {
            result->push_back(node->Key().airport);
        });
        auto node = graph->GetNode({airport_id, db->ParseDateTime(start_time)});
        node->BFS();
        return result;
    }

    struct ConnectivityResult {
    };
    ConnectivityResult query_connectivity(int airport_1, int airport_2);

    struct ShortestPathResult {
    };
    ShortestPathResult query_shortest_path(int airport_1, int airport_2, int start_time, int end_time);

    struct MinimumCostPathResult {
    };
    MinimumCostPathResult query_minimum_cost_path(int airport_1, int airport_2, int start_time, int end_time);

    struct AllPathsResult {
    };
    AllPathsResult query_all_paths(int airport_1, int airport_2, int start_time, int end_time);
};
