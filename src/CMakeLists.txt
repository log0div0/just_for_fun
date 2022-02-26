cmake_minimum_required(VERSION 3.0)

file(GLOB SRCS "*.cpp")

add_executable(just_for_fun ${SRCS})
target_link_libraries(just_for_fun glad glfw stb imgui)
