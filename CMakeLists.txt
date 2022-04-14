project(spoc)

cmake_minimum_required(VERSION 3.0.0)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)

include_directories(./include ./include/json)

macro(add_test name)
    add_executable(${name} ./tests/${name}.cpp)
    target_link_libraries(${name} z)
endmacro()

add_test(test_compress)
add_test(test_extent)
add_test(test_json)
add_test(test_spoc)

macro(add_app name)
    add_executable(${name} ./apps/${name}.cpp)
    target_link_libraries(${name} z)
endmacro()

add_app(text2spoc)
add_app(spoc2text)
add_app(spoc_info)

macro(add_laslib_app name)
    add_executable(${name} ./apps/${name}.cpp)
    target_link_libraries(${name} z ${CMAKE_SOURCE_DIR}/laslib/LASlib/lib/liblas.a)
    include_directories(laslib/LASlib/inc)
    include_directories(laslib/LASzip/src)
endmacro()

add_laslib_app(las2spoc)
add_laslib_app(spoc2las)
