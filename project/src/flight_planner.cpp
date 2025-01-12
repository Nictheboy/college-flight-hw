#include "../include/flight_planner.hpp"

std::shared_ptr<List<Airport>> Planner::EnumerateAirportsDFS(Airport airport, DateTime datetime_from) {
    auto graph = std::make_shared<FlightGraphAirportOnly>(db);
    auto result = std::make_shared<List<Airport>>();
    graph->OnNodeDiscovered.AddListener([&](auto node) {
        result->push_back(node->Key().airport);
    });
    auto node = graph->GetNode({airport, datetime_from});
    node->DFS();
    return result;
}

std::shared_ptr<List<Airport>> Planner::EnumerateAirportsBFS(Airport airport, DateTime datetime_from) {
    auto graph = std::make_shared<FlightGraphAirportOnly>(db);
    auto result = std::make_shared<List<Airport>>();
    graph->OnNodeDiscovered.AddListener([&](auto node) {
        result->push_back(node->Key().airport);
    });
    auto node = graph->GetNode({airport, datetime_from});
    node->BFS();
    return result;
}

Planner::PathList Planner::EnumerateAllPaths(int airport_from, int airport_to, DateTime datetime_from, DateTime datetime_to, int depth_limit) {
    auto graph = std::make_shared<FlightGraphComplete>(db);
    auto from = graph->GetNode({airport_from, datetime_from});
    auto to = graph->GetNode({airport_to, datetime_to});
    auto paths = graph->AllPathsTo(from, to, depth_limit);
    return ConvertPathList(paths);
}

std::optional<Planner::Path> Planner::QueryMinimumTimePath(int airport_from, int airport_to, DateTime datetime_from, DateTime datetime_to) {
    auto graph = std::make_shared<FlightGraphCompleteWithTime>(db);
    auto from = graph->GetNode({airport_from, datetime_from});
    auto to = graph->GetNode({airport_to, datetime_to});
    auto path = graph->BestPathTo(from, to);
    return path.has_value() ? std::make_optional(ConvertPath(path.value())) : std::nullopt;
}

std::optional<Planner::Path> Planner::QueryMinimumCostPath(int airport_from, int airport_to, DateTime datetime_from, DateTime datetime_to) {
    auto graph = std::make_shared<FlightGraphCompleteWithPrice>(db);
    auto from = graph->GetNode({airport_from, datetime_from});
    auto to = graph->GetNode({airport_to, datetime_to});
    auto path = graph->BestPathTo(from, to);
    return path.has_value() ? std::make_optional(ConvertPath(path.value())) : std::nullopt;
}

Planner::Path Planner::ConvertPath(AbstractFlightGraph::Path path) {
    auto result = std::make_shared<Vector<FlightDatabase::Record>>();
    for (auto to = path->begin(), from = to++; to != path->end(); from = to++) {
        auto record = db->QueryRecordByAirportsAndArrivalTime(
            (*from)->Key().airport, (*to)->Key().airport, (*to)->Key().no_sooner_than);
        result->push_back(record);
    }
    return result;
}

Planner::PathList Planner::ConvertPathList(AbstractFlightGraph::PathList path_list) {
    auto result = std::make_shared<List<Path>>();
    for (auto path : *path_list)
        result->push_back(ConvertPath(path));
    return result;
}
