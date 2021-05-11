cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD "20")
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project("imguiwrap")

add_subdirectory(vendor)

add_library(
	imguiwrap

	imguiwrap.cpp
	imguiwrap.h
	imguiwrap.helpers.h
)

target_include_directories(
	imguiwrap

	PUBLIC

	${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(
	imguiwrap

	PUBLIC

	imgui
)

# Are we standalone?
if ("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_SOURCE_DIR}")
	set(IMGUIWRAP_STANDALONE TRUE)
endif()

if (IMGUIWRAP_STANDALONE)
	add_subdirectory(example)
endif ()
