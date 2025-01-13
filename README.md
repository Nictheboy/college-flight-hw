# college-flight-hw
College homework: a flight system

## Usage

Use the following bash to build and run unit tests
```
mkdir build
cd build
cmake ..
make
./unit_test
```

Use the following commands to perform query
```
// query_dfs
> dfs 35 5/5/2017 0:00

// query_bfs
> bfs 35 5/5/2017 0:00

// query_connectivity from 48 to 50
> connectivity 39 10

// query_shortest_path from 18 5/5/2017 0:00 to 52 5/7/2017 0:00
> shortest_path 39 10 5/6/2017 0:00 5/8/2017 0:00

// query_minimum_cost_path from 17 5/7/2017 0:00 to 52 5/9/2017 23:59
> minimum_cost_path 28 74 5/5/2017 0:00 5/9/2017 23:59

// query_all_paths from 48 5/5/2017 12:00 to 50 5/8/2017 12:00
> all_paths 39 52 5/5/2017 0:00 5/9/2017 23:59

// exit the program
> exit
```
