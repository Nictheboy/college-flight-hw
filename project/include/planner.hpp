#pragma once
#include "database.hpp"

class Planner {
   private:
    std::shared_ptr<FlightDatabase> db;

   public:
    Planner(std::shared_ptr<FlightDatabase> db)
        : db(db) {}

    auto query_dfs(int airport_id, std::string start_time) {
        auto node = FlightDatabase::Node(*db, airport_id, db->ParseDateTime(start_time));
        return node.DFS();
    }

    struct BFSResult {
    };
    BFSResult query_bfs(int airport_id, std::string start_time);

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
