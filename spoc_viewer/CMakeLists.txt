cmake_minimum_required(VERSION 3.14)

project(spoc_viewer)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)

find_package(VTK
  COMPONENTS
    FiltersSources
    IOInfovis
    InfovisCore
    InfovisLayout
    InteractionStyle
    RenderingCore
    RenderingLabel
    RenderingOpenGL2
  OPTIONAL_COMPONENTS
    FiltersStatistics
    GUISupportQt
    IOSQL
    ViewsInfovis
    ViewsQt)

if (NOT VTK_FOUND)
    message(FATAL_ERROR "VTK NOT Found")
endif ()

include_directories(./spoc)

add_executable(spoc_viewer spoc_viewer.cpp)
target_link_libraries(spoc_viewer gmp z ${VTK_LIBRARIES})
vtk_module_autoinit(TARGETS spoc_viewer MODULES ${VTK_LIBRARIES})
