cmake_minimum_required(VERSION 3.15)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

if (NOT CMAKE_GENERATOR_PLATFORM MATCHES "^Gaming\\.Xbox")

add_subdirectory(glfw-3.3.6)
add_library(glad glad/glad.c)

add_library(tinyobjloader tinyobjloader/tiny_obj_loader.cc)

target_include_directories(tinyobjloader PUBLIC
	tinyobjloader
)

if (ENABLE_VULKAN)
	file(TO_CMAKE_PATH "$ENV{VK_SDK_PATH}/Source/SPIRV-Reflect/" SPIRV_REFLECT_DIR)
	add_library(spirv_reflect ${SPIRV_REFLECT_DIR}/spirv_reflect.c)
	target_include_directories(spirv_reflect PUBLIC ${SPIRV_REFLECT_DIR})
endif()

set(IMGUI_SRCS
	imgui-1.87/imgui.cpp
	imgui-1.87/imgui_draw.cpp
	imgui-1.87/imgui_tables.cpp
	imgui-1.87/imgui_widgets.cpp
	imgui-1.87/backends/imgui_impl_glfw.cpp
)

if (ENABLE_OPENGL)
	list(APPEND IMGUI_SRCS imgui-1.87/backends/imgui_impl_opengl3.cpp)
endif()

if (ENABLE_VULKAN)
	list(APPEND IMGUI_SRCS imgui-1.87/backends/imgui_impl_vulkan.cpp)
endif()

if (ENABLE_DX12)
	list(APPEND IMGUI_SRCS imgui-1.87/backends/imgui_impl_dx12.cpp)
endif()

add_library(imgui ${IMGUI_SRCS})

target_include_directories(imgui PUBLIC
	imgui-1.87
	imgui-1.87/backends
)

target_link_libraries(imgui glfw)

if (ENABLE_OPENGL)
	add_executable(imgui_demo
		imgui-1.87/imgui_demo.cpp
		imgui-1.87/examples/example_glfw_opengl3/main.cpp
	)

	target_link_libraries(imgui_demo imgui)
	if (WIN32)
		target_link_libraries(imgui_demo opengl32)
	endif()
endif()

endif()
