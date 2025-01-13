#pragma once
#include "abstract_flight_graph.hpp"
#include "flight_database.hpp"

class Planner {
   private:
    std::shared_ptr<FlightDatabase> db;

   public:
    Planner(std::shared_ptr<FlightDatabase> db)
        : db(db) {}

    using PNode = std::shared_ptr<AbstractFlightGraph::Node>;
    using Path = std::shared_ptr<Vector<FlightDatabase::Record>>;
    using PathList = std::shared_ptr<List<Path>>;

    std::shared_ptr<List<Airport>> EnumerateAirportsDFS(Airport airport, DateTime datetime_from);
    std::shared_ptr<List<Airport>> EnumerateAirportsBFS(Airport airport, DateTime datetime_from);
    PathList EnumerateAllPaths(
        int airport_from,
        int airport_to,
        DateTime datetime_from = LONG_LONG_MIN,
        DateTime datetime_to = LONG_LONG_MAX,
        int depth_limit = 2);
    std::optional<Path> QueryMinimumTimePath(
        int airport_from,
        int airport_to,
        DateTime datetime_from = LONG_LONG_MIN,
        DateTime datetime_to = LONG_LONG_MAX);
    std::optional<Path> QueryMinimumCostPath(
        int airport_from,
        int airport_to,
        DateTime datetime_from = LONG_LONG_MIN,
        DateTime datetime_to = LONG_LONG_MAX);

   private:
    Path ConvertPath(AbstractFlightGraph::Path path);
    PathList ConvertPathList(AbstractFlightGraph::PathList path_list);
};
