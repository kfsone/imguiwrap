# imguiwrap provides a cmakelists for incorporating imgui into other projects.
#
# Currently, it assumes an opengl implementation using the vendored glfw library.
#
# This has been the only backend I've had reason to use, and I didn't want to
# do a half-baked job of introducing support for other backends.
#
# Pull-requests that introduce support for selecting one or more alternate backend
# are welcome.
#
cmake_minimum_required(VERSION 3.16)

set (IMGUIWRAP_CXX_STANDARD "17" CACHE STRING "Specify the C++ standard version Imgui will use (must be 17 or higher")

set (CMAKE_CXX_STANDARD "${IMGUIWRAP_CXX_STANDARD}")
set (CMAKE_CXX_STANDARD_REQUIRED ON)

project("imguiwrap")

set (IMGUIWRAP_BACKEND "GL3W" CACHE STRING "Specify the imgui backend")
if (IMGUIWRAP_BACKEND STREQUAL "GL3W")
	add_compile_definitions(IMGUI_IMPL_OPENGL_LOADER_GL3W)
endif ()

add_subdirectory(vendor)

add_library(
	imguiwrap

	imguiwrap.cpp
	imguiwrap.h
	imguiwrap.helpers.h
	imguiwrap.dear.h
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
