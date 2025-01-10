#include <iostream>
#include <memory>
#include <string>
#include "planner.hpp"
#include "surakarta_logger.hpp"

int main() {
    auto logger = std::make_shared<SurakartaLoggerStdout>();
    auto db = std::make_shared<FlightDatabase>("../project/data/flight-data.csv", logger);
    auto planner = std::make_shared<Planner>(db);

    while (true) {
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
            auto result = planner->query_dfs(airport_id, start_time);
            printf("%d", airport_id);
            for (auto& edge : *result) {
                printf(" %d", edge.node->Airport());
            }
            printf("\n");
        }
    }
    return 0;
}
