project(spoc)

cmake_minimum_required(VERSION 3.0.0)

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

include_directories(./include)

macro(add_test name)
    add_executable(${name} ./tests/${name}.cpp)
    target_link_libraries(${name} gmp z)
endmacro()

add_test(test_cmd)
add_test(test_compress)
add_test(test_diff)
add_test(test_extent)
add_test(test_info)
add_test(test_json)
add_test(test_spoc)
add_test(test_spoc2text)
add_test(test_text2spoc)
add_test(test_tile)
add_test(test_transform)
add_test(test_voxel)

macro(add_app name)
    add_executable(spoc_${name} apps/${name}/${name}.cpp)
    target_link_libraries(spoc_${name} gmp z)
endmacro()

add_app(diff)
add_app(info)
add_app(spoc2text)
add_app(srs)
add_app(text2spoc)
add_app(tile)
add_app(transform)

macro(add_laslib_app name)
    add_executable(spoc_${name} ./apps/${name}/${name}.cpp)
    target_link_libraries(spoc_${name} gmp z ${CMAKE_SOURCE_DIR}/laslib/LASlib/lib/liblas.a)
    include_directories(laslib/LASlib/inc)
    include_directories(laslib/LASzip/src)
endmacro()

add_laslib_app(las2spoc)
add_laslib_app(spoc2las)

macro(add_laslib_test name)
    add_executable(${name} ./tests/${name}.cpp)
    target_link_libraries(${name} gmp z ${CMAKE_SOURCE_DIR}/laslib/LASlib/lib/liblas.a)
    include_directories(laslib/LASlib/inc)
    include_directories(laslib/LASzip/src)
endmacro()

add_laslib_test(test_las2spoc)
add_laslib_test(test_spoc2las)
