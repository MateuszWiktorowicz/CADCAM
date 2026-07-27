cmake_minimum_required(VERSION 3.24)

project(ToolpathStudio
    VERSION 0.1.0
    DESCRIPTION "C++20 CAD/CAM prototype for desktop CNC toolpath generation"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(TOOLPATH_BUILD_APP "Build the Qt6 desktop application" ON)
option(TOOLPATH_BUILD_CLI "Build the command line sample generator" ON)
option(TOOLPATH_BUILD_CAD "Build OpenCascade CAD integration" ON)
option(TOOLPATH_BUILD_TESTS "Build GoogleTest tests" ON)

find_package(Eigen3 REQUIRED NO_MODULE)

add_subdirectory(src/core)

if(TOOLPATH_BUILD_CAD)
    add_subdirectory(src/cad)
endif()

if(TOOLPATH_BUILD_CLI)
    add_subdirectory(src/cli)
endif()

if(TOOLPATH_BUILD_APP)
    add_subdirectory(src/app)
endif()

if(TOOLPATH_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

