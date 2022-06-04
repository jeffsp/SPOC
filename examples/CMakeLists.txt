project(examples)

cmake_minimum_required(VERSION 3.0.0)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)

find_package(OpenMP REQUIRED)

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
endif()

set(CMAKE_CXX_FLAGS "-Wall -Werror -Wno-class-memaccess ${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")

include_directories(../include)

macro(add_example name)
    add_executable(${name} ./${name}.cpp)
    target_link_libraries(${name} gmp z)
endmacro()


add_example(noop)
