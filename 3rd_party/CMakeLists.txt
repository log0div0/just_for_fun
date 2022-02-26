cmake_minimum_required(VERSION 3.15)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(glfw-3.3.6)

add_library(glad glad/glad.c)

add_library(imgui
	imgui-1.87/imgui.cpp
	imgui-1.87/imgui_draw.cpp
	imgui-1.87/imgui_tables.cpp
	imgui-1.87/imgui_widgets.cpp
	imgui-1.87/backends/imgui_impl_glfw.cpp
	imgui-1.87/backends/imgui_impl_opengl3.cpp
)

target_include_directories(imgui PUBLIC
	imgui-1.87
	imgui-1.87/backends
)

target_link_libraries(imgui glfw)