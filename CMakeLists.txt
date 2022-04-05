project(spoc)

cmake_minimum_required(VERSION 3.0.0)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)

macro(add_app name)
    add_executable(${name} ${name}.cpp)
    target_link_libraries(${name} z)
endmacro()

add_app(test_compress)
add_app(test_extent)
add_app(test_spoc)
add_app(text2spoc)
add_app(spoc2text)

macro(add_laslib_app name)
    add_executable(${name} ${name}.cpp)
    target_link_libraries(${name} z ${CMAKE_SOURCE_DIR}/laslib/LASlib/lib/liblas.a)
    include_directories(laslib/LASlib/inc)
    include_directories(laslib/LASzip/src)
endmacro()

add_laslib_app(las2spoc)
