cmake_minimum_required(VERSION 3.0.0)
project(spoc_conversion)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 23)

find_package(OpenMP REQUIRED)

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
endif()

set(CMAKE_CXX_FLAGS "-Wall -Werror -Wno-class-memaccess ${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")

include_directories(${PROJECT_SOURCE_DIR} ./include ../include)

macro(add_unit_test name)
    add_executable(${name} ./tests/unit/${name}.cpp)
    target_link_libraries(${name} ${OPENMP_LIBRARIES} z ${CMAKE_SOURCE_DIR}/laslib/LASlib/lib/liblas.a)
    target_include_directories(${name} PRIVATE laslib/LASlib/inc)
    target_include_directories(${name} PRIVATE laslib/LASzip/src)
endmacro()

add_unit_test(test_spoc_conversion)

macro(add_app_test name)
    add_executable(test_${name} tests/app/test_${name}.cpp)
    target_link_libraries(test_${name} ${OPENMP_LIBRARIES} z)
    target_include_directories(test_${name} PRIVATE apps/${name})
endmacro()

add_app_test(spoc2text)
add_app_test(text2spoc)

macro(add_app name)
    add_executable(${name} apps/${name}/${name}.cpp)
    target_link_libraries(${name} ${OPENMP_LIBRARIES} z)
endmacro()

add_app(spoc2text)
add_app(text2spoc)

macro(add_laslib_app name)
    add_executable(${name} ./apps/${name}/${name}.cpp)
    target_link_libraries(${name} ${OPENMP_LIBRARIES} z ${CMAKE_SOURCE_DIR}/laslib/LASlib/lib/liblas.a)
    target_include_directories(${name} PRIVATE laslib/LASlib/inc)
    target_include_directories(${name} PRIVATE laslib/LASzip/src)
endmacro()

add_laslib_app(las2spoc)
add_laslib_app(spoc2las)

macro(add_laslib_test name)
    add_executable(test_${name} ./tests/app/test_${name}.cpp)
    target_link_libraries(test_${name} ${OPENMP_LIBRARIES} z ${CMAKE_SOURCE_DIR}/laslib/LASlib/lib/liblas.a)
    target_include_directories(test_${name} PRIVATE laslib/LASlib/inc)
    target_include_directories(test_${name} PRIVATE laslib/LASzip/src)
    target_include_directories(test_${name} PRIVATE apps/${name})
endmacro()

add_laslib_test(las2spoc)
add_laslib_test(spoc2las)
