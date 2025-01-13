#include <catch2/catch_test_macros.hpp>
#include "../project/include/flight_planner.hpp"

auto PathToString(Planner::Path path) {
    auto str = std::string();
    for (auto &record : *path) {
        str += std::to_string(record.id) + " ";
    }
    return str;
}

auto PathListToString(Planner::PathList path_list) {
    auto str = std::string();
    for (auto path : *path_list) {
        str += PathToString(path) + "; ";
    }
    return str;
}

TEST_CASE("test flight", "[flight]") {
    auto db = std::make_shared<FlightDatabase>("../project/data/flight-data.csv");
    auto planner = std::make_shared<Planner>(db);

    SECTION("test dfs") {
        {
            auto result = planner->EnumerateAirportsDFS(35, db->ParseDateTime("5/5/2017 0:00"));
            auto str = std::string();
            for (auto airport : *result) {
                str += std::to_string(airport) + " ";
            }
            REQUIRE(str == "35 34 49 62 50 25 63 14 32 71 38 27 31 61 11 15 52 78 77 67 36 72 66 37 43 46 45 73 57 44 75 59 12 68 76 60 26 79 56 54 30 48 47 7 70 42 3 8 22 16 19 20 6 51 21 9 41 33 24 18 40 17 2 65 58 64 10 39 1 53 55 13 23 ");
        }
    };

    SECTION("test bfs") {
        {
            auto result = planner->EnumerateAirportsBFS(35, db->ParseDateTime("5/5/2017 0:00"));
            auto str = std::string();
            for (auto airport : *result) {
                str += std::to_string(airport) + " ";
            }
            REQUIRE(str == "35 34 50 49 48 61 52 18 14 40 60 33 62 31 27 17 36 72 11 67 37 25 32 2 66 65 56 41 57 79 38 58 68 46 7 8 54 16 21 22 64 9 15 78 30 10 42 6 20 39 63 1 24 53 12 55 73 13 23 59 5 3 19 51 76 26 44 77 43 45 47 71 75 28 29 70 69 74 ");
        }
    };

    SECTION("test connectivity") {
        {
            auto result = planner->EnumerateAllPaths(39, 10);
            auto str = PathListToString(result);
            REQUIRE(str == "2300 1369 ; 2300 1370 ; ");
        }
    };

    SECTION("test shortest_path") {
        {
            auto result = planner->QueryMinimumTimePath(39, 10, db->ParseDateTime("5/6/2017 0:00"), db->ParseDateTime("5/8/2017 0:00"));
            auto str = PathToString(result.value());
            REQUIRE(str == "2300 1369 ");
        }
    }

    SECTION("test minimum_cost_path") {
        {
            auto result = planner->QueryMinimumCostPath(28, 74, db->ParseDateTime("5/5/2017 0:00"), db->ParseDateTime("5/9/2017 23:59"));
            auto str = PathToString(result.value());
            REQUIRE(str == "2113 471 1651 1733 ");
        }
        {
            REQUIRE_THROWS(planner->QueryMinimumCostPath(80, 1, db->ParseDateTime("5/10/2017 0:00"), db->ParseDateTime("10/0/2017 23:59")));
        }
    }

    SECTION("test all_paths") {
        {
            auto result = planner->EnumerateAllPaths(39, 52, db->ParseDateTime("5/5/2017 0:00"), db->ParseDateTime("5/9/2017 23:59"));
            REQUIRE(result->size() == 8);
        }
    };
}
