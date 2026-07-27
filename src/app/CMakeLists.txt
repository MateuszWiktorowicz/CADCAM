find_package(Qt6 REQUIRED COMPONENTS Widgets OpenGLWidgets)
find_package(OpenGL REQUIRED)

qt_standard_project_setup()

qt_add_executable(toolpath_app
    main.cpp
    MainWindow.cpp
    MainWindow.hpp
    ToolpathView.cpp
    ToolpathView.hpp
)

target_link_libraries(toolpath_app
    PRIVATE
        Qt6::Widgets
        Qt6::OpenGLWidgets
        OpenGL::GL
        ToolpathStudio::Core
)

if(TARGET ToolpathStudio::Cad)
    target_link_libraries(toolpath_app PRIVATE ToolpathStudio::Cad)
endif()

target_compile_features(toolpath_app PRIVATE cxx_std_20)

set_target_properties(toolpath_app PROPERTIES
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

