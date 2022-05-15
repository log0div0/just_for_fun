cmake_minimum_required(VERSION 3.15)

project(just_for_fun)

set(CMAKE_CXX_STANDARD 20)

if(WIN32)
	add_definitions(
		-D_WIN32_WINNT=0x0A00
		-D_SCL_SECURE_NO_WARNINGS
		-D_WINSOCK_DEPRECATED_NO_WARNINGS
		-D_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
		-D_SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
		-DNOMINMAX
		-DUNICODE
		-D_UNICODE
	)
endif()

if (WIN32)
	add_definitions(-D__PRETTY_FUNCTION__=__FUNCSIG__)
endif()

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Debug")
endif()

if(WIN32)
	set(CMAKE_CONFIGURATION_TYPES "${CMAKE_BUILD_TYPE}")
	set(CMAKE_TRY_COMPILE_CONFIGURATION "${CMAKE_BUILD_TYPE}")
endif()

if(WIN32)
	add_compile_options($<$<CXX_COMPILER_ID:MSVC>:/MP>)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out/sbin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out/lib)

include_directories(${CMAKE_SOURCE_DIR}/3rd_party)
include_directories(${CMAKE_SOURCE_DIR}/lib)

include_directories($ENV{VK_SDK_PATH}/include)
link_directories($ENV{VK_SDK_PATH}/lib)

add_subdirectory(3rd_party)
add_subdirectory(lib)
add_subdirectory(src)
