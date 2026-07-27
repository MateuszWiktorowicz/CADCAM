find_package(GTest CONFIG REQUIRED)

add_executable(toolpath_tests
    ProfileOperationTests.cpp
    SketchGeometryTests.cpp
)

target_link_libraries(toolpath_tests
    PRIVATE
        ToolpathStudio::Core
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(toolpath_tests)
