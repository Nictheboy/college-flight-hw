cmake_minimum_required(VERSION 3.10)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(lab
    LANGUAGES CXX C
    DESCRIPTION "Data structure course lab."
)

# add_compile_options(-fsanitize=address)
# add_compile_options(-g)
# add_link_options(-fsanitize=address)

# To mitigate a bug maybe related to lifecycle of shared_ptr
set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)

# -------------------------- miniSTL --------------------------

set(miniSTL_SOURCE_DIR miniSTL)
add_subdirectory(${miniSTL_SOURCE_DIR})

# set(miniSTL_DIR "D:/Projects/cpp/hyx-lab/miniSTL/lib/share/miniSTL/cmake")
# find_package(miniSTL REQUIRED)

# -------------------------- Catch2 --------------------------

set(catch2_SOURCE_DIR Catch2)
add_subdirectory(${catch2_SOURCE_DIR})
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)

#--------------------------- Project -----------------------

file(GLOB airplane_sources project/src/*.cpp project/include/*.hpp)

add_executable(airplane ${airplane_sources})

target_include_directories(airplane PRIVATE project/include)
target_link_libraries(airplane PRIVATE miniSTL::miniSTL)

# -------------------------- Test --------------------------

file(GLOB test_sources test/*.cpp project/src/*.cpp project/include/*.hpp)
list(FILTER test_sources EXCLUDE REGEX ".*/main\\.cpp$")

add_executable(unit_test ${test_sources})
target_link_libraries(unit_test PRIVATE miniSTL::miniSTL Catch2::Catch2WithMain)

include(CTest)
include(Catch)
catch_discover_tests(unit_test)
