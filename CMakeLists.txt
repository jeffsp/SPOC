cmake_minimum_required(VERSION 3.0.0)
project(spoc)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)

find_package(OpenMP REQUIRED)

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
endif()

option(BUILD_COVERAGE "Build for coverage testing" OFF)

if(BUILD_COVERAGE)
   set(CMAKE_CXX_FLAGS "-g -O0 -fprofile-arcs -ftest-coverage -fPIC -Wall -Werror -Wno-class-memaccess ${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
else()
    set(CMAKE_CXX_FLAGS "-Wall -Werror -Wno-class-memaccess ${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

include_directories(${PROJECT_SOURCE_DIR})

macro(add_unit_test name)
    add_executable(${name} ./tests/unit/${name}.cpp)
    target_link_libraries(${name} ${OPENMP_LIBRARIES} z)
endmacro()

add_unit_test(test_app_utils)
add_unit_test(test_asprs)
add_unit_test(test_benchmark_utils)
add_unit_test(test_compress)
add_unit_test(test_contracts)
add_unit_test(test_cmd)
add_unit_test(test_extent)
add_unit_test(test_file)
add_unit_test(test_header)
add_unit_test(test_io)
add_unit_test(test_json)
add_unit_test(test_point)
add_unit_test(test_point_record)
add_unit_test(test_test_utils)
add_unit_test(test_subsampling)
add_unit_test(test_utils)
add_unit_test(test_voxel)
add_unit_test(test_radius_search)

macro(add_benchmark name)
    add_executable(${name} ./benchmarks/${name}.cpp)
    target_link_libraries(${name} ${OPENMP_LIBRARIES} z)
endmacro()

add_benchmark(benchmark_io)
add_benchmark(benchmark_spoc_file)

macro(add_app_test name)
    add_executable(test_${name} tests/app/test_${name}.cpp)
    target_link_libraries(test_${name} ${OPENMP_LIBRARIES} z)
    target_include_directories(test_${name} PRIVATE apps/${name})
endmacro()

add_app_test(diff)
add_app_test(filter)
add_app_test(info)
add_app_test(merge)
add_app_test(tile)
add_app_test(tool)
add_app_test(transform)

macro(add_app name)
    add_executable(spoc_${name} apps/${name}/${name}.cpp)
    target_link_libraries(spoc_${name} ${OPENMP_LIBRARIES} z)
endmacro()

add_app(compress)
add_app(decompress)
add_app(diff)
add_app(filter)
add_app(info)
add_app(merge)
add_app(srs)
add_app(tile)
add_app(tool)
add_app(transform)
