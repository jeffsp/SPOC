project(overstrike)

cmake_minimum_required(VERSION 3.0.0)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)

macro(add_app name)
    add_executable(${name} ${name}.cpp)
    target_link_libraries(${name} fmt z)
endmacro()

add_app(test_compress)
add_app(test_extent)
add_app(test_octree)
add_app(text2spc)
add_app(spc2text)
