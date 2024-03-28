cmake_minimum_required (VERSION 3.16)

set (CMAKE_CXX_STANDARD "${IMGUIWRAP_CXX_STANDARD}")
set (CMAKE_CXX_STANDARD_REQUIRED ON)

project ("imgui")

set (IMGUI_PLATFORM_BACKEND "glfw" CACHE STRING "Specify which imgui platform backend to use")
set (IMGUI_RENDERER_BACKEND "opengl3" CACHE STRING "Specify which imgui renderer backend to use")

find_package (OpenGL REQUIRED)

set (
	IMGUI_SOURCES

	src/imgui.cpp
	src/imgui.h
	src/imconfig.h
	src/imgui_demo.cpp
	src/imgui_draw.cpp
	src/imgui_internal.h
	src/imgui_tables.cpp
	src/imgui_widgets.cpp
)
set (
	IMGUI_PLATFORM_SOURCES

	src/backends/imgui_impl_${IMGUI_PLATFORM_BACKEND}.cpp
	src/backends/imgui_impl_${IMGUI_PLATFORM_BACKEND}.h
)
set (
	IMGUI_RENDERER_SOURCES

	src/backends/imgui_impl_${IMGUI_RENDERER_BACKEND}.cpp
	src/backends/imgui_impl_${IMGUI_RENDERER_BACKEND}.h
)

add_library (
	imgui

	${IMGUI_SOURCES}
	${IMGUI_PLATFORM_SOURCES}
	${IMGUI_RENDERER_SOURCES}
)

target_include_directories (
	imgui

	SYSTEM PUBLIC

	${CMAKE_CURRENT_SOURCE_DIR}/src
	${CMAKE_CURRENT_SOURCE_DIR}/src/backends
	$<$<BOOL:${IMGUI_GLFW_PATH}>:${IMGUI_GLFW_PATH}/include>
)

target_link_libraries (
	imgui

	PUBLIC

	glfw
	OpenGL::GL
)
