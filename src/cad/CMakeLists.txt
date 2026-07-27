find_package(OpenCASCADE REQUIRED)

add_library(toolpath_cad
    CartesianCoordinateSystem.cpp
    StepLoader.cpp
)

add_library(ToolpathStudio::Cad ALIAS toolpath_cad)

target_include_directories(toolpath_cad
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
        ${OpenCASCADE_INCLUDE_DIR}
)

target_link_libraries(toolpath_cad
    PUBLIC
        ToolpathStudio::Core
        ${OpenCASCADE_LIBRARIES}
)

target_compile_features(toolpath_cad PUBLIC cxx_std_20)
