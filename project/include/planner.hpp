#include <miniSTL/stl.hpp>
#include <string>
#include <tuple>

struct DFSResult {
    
};

struct BFSResult {

};


struct ConnectivityResult {

};

struct ShortestPathResult {

};

struct MinimumCostPathResult {

};

struct AllPathsResult {

};

struct Planner {

    DFSResult query_dfs(int airport_id, std::string start_time);
    BFSResult query_bfs(int airport_id, std::string start_time);
    ConnectivityResult query_connectivity(int airport_1, int airport_2);
    ShortestPathResult query_shortest_path(int airport_1, int airport_2, int start_time, int end_time);
    MinimumCostPathResult query_minimum_cost_path(int airport_1, int airport_2, int start_time, int end_time);
    AllPathsResult query_all_paths(int airport_1, int airport_2, int start_time, int end_time);
};
