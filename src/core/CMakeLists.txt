add_library(toolpath_core
    GCodeWriter.cpp
    ProfileOperation.cpp
)

add_library(ToolpathStudio::Core ALIAS toolpath_core)

target_include_directories(toolpath_core
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(toolpath_core
    PUBLIC
        Eigen3::Eigen
)

target_compile_features(toolpath_core PUBLIC cxx_std_20)

