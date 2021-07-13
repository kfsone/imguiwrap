cmake_minimum_required(VERSION 3.16)

set (IMGUIWRAP_CXX_STANDARD "17" CACHE STRING "Specify the C++ standard version Imgui will use (must be 17 or higher")

set (CMAKE_CXX_STANDARD "${IMGUIWRAP_CXX_STANDARD}")
set (CMAKE_CXX_STANDARD_REQUIRED ON)

project("imguiwrap")

add_subdirectory(vendor)

add_library(
	imguiwrap

	imguiwrap.cpp
	imguiwrap.h
	imguiwrap.helpers.h
	imguiwrap.dear.h
)

find_package(GLEW REQUIRED)

target_include_directories(
	imguiwrap

	PUBLIC

	${CMAKE_CURRENT_SOURCE_DIR}
	${GLEW_INCLUDE_DIRS}
)

target_link_libraries(
	imguiwrap

	PUBLIC

	imgui
	${GLEW_LIBRARIES}
)

if (WIN32)
	string (REGEX REPLACE "/EH[a-z]+-?" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
	set (IMGW_NO_EXCEPTIONS /EHc-)
	set (IMGW_ALL_WARNINGS  /W3)
	set (IMGW_NO_RTTI /GR-)
else ()
	set (IMGW_NO_EXCEPTIONS -fno-exceptions)
	set (IMGW_ALL_WARNINGS  -Wall -Werror -Wextra -Wpedantic)
	set (IMGW_NO_RTTI -fno-rtti)
endif ()

target_compile_options(imguiwrap PRIVATE ${IMGW_NO_RTTI} ${IMGW_NO_EXCEPTIONS} ${IMGW_ALL_WARNINGS})

# Are we standalone?
if ("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_SOURCE_DIR}")
	set(IMGUIWRAP_STANDALONE TRUE)
endif()

if (IMGUIWRAP_STANDALONE)
	add_subdirectory(example)
endif ()
