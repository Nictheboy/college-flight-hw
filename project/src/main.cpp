#include <iostream>
#include <memory>
#include <string>
#include "../include/flight_planner.hpp"
#include "../include/surakarta_logger.hpp"

int main() {
    auto logger = std::make_shared<SurakartaLoggerStdout>();
    auto db = std::make_shared<FlightDatabase>("../project/data/flight-data.csv", logger);
    auto planner = std::make_shared<Planner>(db);

    while (!std::cin.eof()) {
        printf("> ");
        std::string query;
        std::getline(std::cin, query);
        if (query == "exit")
            break;
        if (query.starts_with("dfs")) {
            query = query.substr(4);
            auto airport_id = std::stoi(query.substr(0, query.find(' ')));
            query = query.substr(query.find(' ') + 1);
            auto start_time = db->ParseDateTime(query);
            auto result = planner->EnumerateAirportsDFS(airport_id, start_time);
            for (auto airport : *result) {
                printf("%d ", airport);
            }
            printf("\n");
        }
        if (query.starts_with("bfs")) {
            query = query.substr(4);
            auto airport_id = std::stoi(query.substr(0, query.find(' ')));
            query = query.substr(query.find(' ') + 1);
            auto start_time = db->ParseDateTime(query);
            auto result = planner->EnumerateAirportsBFS(airport_id, start_time);
            for (auto airport : *result) {
                printf("%d ", airport);
            }
            printf("\n");
        }
        if (query.starts_with("connectivity")) {
            query = query.substr(13);
            auto airport_from = std::stoi(query.substr(0, query.find(' ')));
            query = query.substr(query.find(' ') + 1);
            auto airport_to = std::stoi(query.substr(0, query.find(' ')));
            auto result = planner->EnumerateAllPaths(airport_from, airport_to);
            for (auto path : *result) {
                for (auto record : *path) {
                    printf("%d ", record.id);
                }
                printf("\n");
            }
        }
        if (query.starts_with("all_paths")) {
            query = query.substr(10);
            auto airport_from = std::stoi(query.substr(0, query.find(' ')));
            query = query.substr(query.find(' ') + 1);
            auto airport_to = std::stoi(query.substr(0, query.find(' ')));
            query = query.substr(query.find(' ') + 1);
            auto date_from = query.substr(0, query.find(' '));
            query = query.substr(query.find(' ') + 1);
            auto time_from = query.substr(0, query.find(' '));
            query = query.substr(query.find(' ') + 1);
            auto date_to = query.substr(0, query.find(' '));
            query = query.substr(query.find(' ') + 1);
            auto time_to = query;
            auto datetime_from = db->ParseDateTime(date_from + " " + time_from);
            auto datetime_to = db->ParseDateTime(date_to + " " + time_to);
            auto result = planner->EnumerateAllPaths(airport_from, airport_to, datetime_from, datetime_to);
            for (auto path : *result) {
                for (auto record : *path) {
                    printf("%d ", record.id);
                }
                printf("\n");
            }
        }
    }
    return 0;
}
