#include <catch2/catch_test_macros.hpp>
#include "../project/include/flight_planner.hpp"

TEST_CASE("test flight", "[flight]") {
    auto db = std::make_shared<FlightDatabase>("../project/data/flight-data.csv");
    auto planner = std::make_shared<Planner>(db);

    SECTION("test dfs") {
        {
            auto result = planner->EnumerateAirportsDFS(48, db->ParseDateTime("5/5/2017 12:20"));
            auto str = std::string();
            for (auto airport : *result) {
                str += std::to_string(airport) + " ";
            }
            REQUIRE(str == "48 49 33 67 27 31 50 38 30 52 36 62 25 61 66 32 57 76 56 54 75 37 77 26 60 79 78 58 59 65 55 64 43 46 45 73 44 29 28 71 47 68 70 72 7 11 14 63 12 15 35 34 48 42 4 41 22 8 16 9 5 6 21 3 19 20 51 24 69 74 53 10 18 40 17 2 39 1 13 23 ");
        }
    };

    SECTION("test bfs") {
        {
            auto result = planner->EnumerateAirportsBFS(48, db->ParseDateTime("5/5/2017 12:20"));
            auto str = std::string();
            for (auto airport : *result) {
                str += std::to_string(airport) + " ";
            }
            REQUIRE(str == "48 49 50 38 35 33 66 54 25 31 62 65 67 32 56 52 60 41 72 57 79 53 37 58 68 46 7 27 16 36 14 21 34 22 48 55 73 63 15 78 30 10 42 6 20 11 18 40 17 2 8 64 9 39 1 24 12 13 23 43 44 45 61 75 71 5 3 19 51 77 26 76 59 4 47 70 69 74 29 28 ");
        }
    };

    SECTION("test connectivity") {
        {
            auto result = planner->EnumerateAllPaths(48, 50);
            REQUIRE(result->size() == 19);
        }
    };

    SECTION("test all_paths") {
        {
            auto result = planner->EnumerateAllPaths(48, 50, db->ParseDateTime("5/5/2017 12:00"), db->ParseDateTime("5/8/2017 12:00"));
            REQUIRE(result->size() == 13);
        }
    };
}
