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
            auto start_time = query;
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
            auto start_time = query;
            auto result = planner->EnumerateAirportsBFS(airport_id, start_time);
            for (auto airport : *result) {
                printf("%d ", airport);
            }
            printf("\n");
        }
    }
    return 0;
}
