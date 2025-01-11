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

int main() {
    while (!std::cin.eof()) {
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
            for (auto path : *result) {
                for (auto record : *path) {
                    printf("%d ", record.id);
                }
                printf("\n");
            }
        } else if (operation == "all_paths") {
            auto airport_from = ReadInt(query);
            auto airport_to = ReadInt(query);
            auto datetime_from = ReadDateTime(query);
            auto datetime_to = ReadDateTime(query);
            auto result = planner->EnumerateAllPaths(airport_from, airport_to, datetime_from, datetime_to);
            for (auto path : *result) {
                for (auto record : *path) {
                    printf("%d ", record.id);
                }
                printf("\n");
            }
        } else if (operation == "minimum_cost_path") {
            auto airport_from = ReadInt(query);
            auto airport_to = ReadInt(query);
            auto datetime_from = ReadDateTime(query);
            auto datetime_to = ReadDateTime(query);
            auto result = planner->QueryMinimumCostPath(airport_from, airport_to, datetime_from, datetime_to);
            if (result.has_value())
                for (auto record : *result.value()) {
                    printf("%d ", record.id);
                }
            else
                printf("No path found");
            printf("\n");
        } else if (operation == "shortest_path") {
            auto airport_from = ReadInt(query);
            auto airport_to = ReadInt(query);
            auto datetime_from = ReadDateTime(query);
            auto datetime_to = ReadDateTime(query);
            auto result = planner->QueryMinimumTimePath(airport_from, airport_to, datetime_from, datetime_to);
            if (result.has_value())
                for (auto record : *result.value()) {
                    printf("%d ", record.id);
                }
            else
                printf("No path found");
            printf("\n");
        } else {
            if (!operation.empty())
                printf("Unknown operation: %s", operation.c_str());
            failed = true;
        }
    }
    return 0;
}
