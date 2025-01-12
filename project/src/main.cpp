#include <iostream>
#include <memory>
#include <string>
#include "../include/flight_planner.hpp"

auto db = std::make_shared<FlightDatabase>("../project/data/flight-data.csv");
auto planner = std::make_shared<Planner>(db);

static std::string ReadToken(std::string& query) {
    auto token = query.substr(0, query.find(' '));
    query = query.substr(query.find(' ') + 1);
    return token;
}

static int ReadInt(std::string& query) {
    return std::stoi(ReadToken(query));
}

static DateTime ReadDateTime(std::string& query) {
    auto date = ReadToken(query);
    auto time = ReadToken(query);
    return db->ParseDateTime(date + " " + time);
}

static std::string DateTimeToString(DateTime datetime) {
    auto date = datetime / 10000;
    auto time = datetime % 10000;
    auto year = date / 10000;
    auto month = (date / 100) % 100;
    auto day = date % 100;
    auto hour = time / 100;
    auto minute = time % 100;
    return std::to_string(year) + "/" + std::to_string(month) + "/" + std::to_string(day) + " " +
           std::to_string(hour) + ":" + std::to_string(minute);
}

static void PrintPath(Planner::Path path) {
    printf("( %d )", path->at(0).airport_from);
    for (auto& record : *path) {
        auto datetime_from = DateTimeToString(record.datetime_from);
        auto datetime_to = DateTimeToString(record.datetime_to);
        printf(" => %s [ID %d $%d] %s => ( %d )", datetime_from.c_str(), record.id, record.price,
               datetime_to.c_str(), record.airport_to);
    }
    printf("\n");
}

int main() {
    while (!std::cin.eof()) {
        try {
            printf("> ");
            std::string query;
            std::getline(std::cin, query);
            auto operation = ReadToken(query);

            bool failed = false;
            if (operation == "exit") {
                break;
            } else if (operation == "dfs") {
                auto airport = ReadInt(query);
                auto start_time = ReadDateTime(query);
                auto result = planner->EnumerateAirportsDFS(airport, start_time);
                for (auto airport : *result) {
                    printf("%d ", airport);
                }
                printf("\n");
            } else if (operation == "bfs") {
                auto airport = ReadInt(query);
                auto start_time = ReadDateTime(query);
                auto result = planner->EnumerateAirportsBFS(airport, start_time);
                for (auto airport : *result) {
                    printf("%d ", airport);
                }
                printf("\n");
            } else if (operation == "connectivity") {
                auto airport_from = ReadInt(query);
                auto airport_to = ReadInt(query);
                auto result = planner->EnumerateAllPaths(airport_from, airport_to);
                for (auto path : *result)
                    PrintPath(path);
            } else if (operation == "all_paths") {
                auto airport_from = ReadInt(query);
                auto airport_to = ReadInt(query);
                auto datetime_from = ReadDateTime(query);
                auto datetime_to = ReadDateTime(query);
                auto result = planner->EnumerateAllPaths(airport_from, airport_to, datetime_from, datetime_to);
                for (auto path : *result)
                    PrintPath(path);
            } else if (operation == "minimum_cost_path") {
                auto airport_from = ReadInt(query);
                auto airport_to = ReadInt(query);
                auto datetime_from = ReadDateTime(query);
                auto datetime_to = ReadDateTime(query);
                auto result = planner->QueryMinimumCostPath(airport_from, airport_to, datetime_from, datetime_to);
                if (result.has_value())
                    PrintPath(result.value());
                else
                    printf("No path found\n");
            } else if (operation == "shortest_path") {
                auto airport_from = ReadInt(query);
                auto airport_to = ReadInt(query);
                auto datetime_from = ReadDateTime(query);
                auto datetime_to = ReadDateTime(query);
                auto result = planner->QueryMinimumTimePath(airport_from, airport_to, datetime_from, datetime_to);
                if (result.has_value())
                    PrintPath(result.value());
                else
                    printf("No path found\n");
            } else {
                if (!operation.empty())
                    printf("Unknown operation: %s\n", operation.c_str());
                failed = true;
            }
        } catch (const std::exception& e) {
            printf("Error: %s\n", e.what());
        }
    }
    return 0;
}
