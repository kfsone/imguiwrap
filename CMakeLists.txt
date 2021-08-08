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

set (IMGUIWRAP_CXX_STANDARD "17" CACHE STRING "Specify the C++ standard to use (must be 17 or higher")

set (CMAKE_CXX_STANDARD "${IMGUIWRAP_CXX_STANDARD}")
set (CMAKE_CXX_STANDARD_REQUIRED ON)

set (IMGUIWRAP_BACKEND "GL3W" CACHE STRING "Specify the imgui backend")
if (IMGUIWRAP_BACKEND STREQUAL "GL3W")
	add_compile_definitions(IMGUI_IMPL_OPENGL_LOADER_GL3W)
endif ()

project ("imguiwrap")

add_subdirectory(vendor)

add_subdirectory(src)
