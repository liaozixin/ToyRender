cmake_minimum_required(VERSION 3.0...3.20 FATAL_ERROR)

project(IMGUI VERSION 3.3.8 LANGUAGES C)

set(CMAKE_CXX_STANDARD 11)
set(SOURCE_FILES 
    ./imgui.cpp
    ./imgui_demo.cpp
    ./imgui_draw.cpp
    ./imgui_tables.cpp
    ./imgui_widgets.cpp
    ./backends/imgui_impl_vulkan.cpp
    ./backends/imgui_impl_glfw.cpp)

set(HEAD_FILES
    ./imgui.h
    ./imgui_internal.h
    ./imconfig.h
    ./imstb_rectpack.h
    ./imstb_textedit.h
    ./imstb_truetype.h
    ./backends/imgui_impl_vulkan.h
    ./backends/imgui_impl_glfw.h)

add_library(imgui STATIC ${SOURCE_FILES})
target_include_directories(imgui PUBLIC ${VULKAN_HEAD_PATH})
target_include_directories(imgui PUBLIC ${GLFW_HEAD_PATH})
target_link_libraries(imgui ${VULKAN_LIB_PATH}/glfw3.lib)
target_link_libraries(imgui ${GLFW_LIB_PATH}/vulkan-1.lib)

install(FILES ${HEAD_FILES}
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        COMPONENT imguicomponent)

install(TARGETS imgui
        DESTINATION ${CMAKE_INSTALL_LIBDIR}
        COMPONENT imguicomponent)


