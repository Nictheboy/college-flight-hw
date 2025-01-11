#pragma once
#include "flight_graph_complete.hpp"

class Planner {
   private:
    std::shared_ptr<FlightDatabase> db;

   public:
    Planner(std::shared_ptr<FlightDatabase> db)
        : db(db) {}

    void SyncAirportDiscoveryAndVisitation(std::shared_ptr<AbstractFlightGraph> graph) {
        graph->OnNodeDiscovered.AddListener([&](auto node) {
            auto airport = node.Key().airport;
            auto other_records_id = db->QueryRecordIdsByAirportTo(airport);
            for (auto record_id : *other_records_id) {
                auto record = db->QueryRecordById(record_id);
                auto node = graph->GetNode({record.airport_to, record.datetime_to});
                node->status = AbstractFlightGraph::Node::Status::DISCOVERED;
            }
        });
        graph->OnNodeVisited.AddListener([&](auto node) {
            auto airport = node.Key().airport;
            auto other_records_id = db->QueryRecordIdsByAirportTo(airport);
            for (auto record_id : *other_records_id) {
                auto record = db->QueryRecordById(record_id);
                auto node = graph->GetNode({record.airport_to, record.datetime_to});
                node->status = AbstractFlightGraph::Node::Status::VISITED;
            }
        });
    }

    auto query_dfs(int airport_id, std::string start_time) {
        auto graph = std::make_shared<FlightGraphComplete>(db);
        SyncAirportDiscoveryAndVisitation(graph);
        auto node = graph->GetNode({airport_id, db->ParseDateTime(start_time)});
        return node->DFS();
    }

    auto query_bfs(int airport_id, std::string start_time) {
        auto graph = std::make_shared<FlightGraphComplete>(db);
        SyncAirportDiscoveryAndVisitation(graph);
        auto node = graph->GetNode({airport_id, db->ParseDateTime(start_time)});
        return node->BFS();
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
