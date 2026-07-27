add_executable(toolpath_cli main.cpp)

target_link_libraries(toolpath_cli
    PRIVATE
        ToolpathStudio::Core
)

target_compile_features(toolpath_cli PRIVATE cxx_std_20)

