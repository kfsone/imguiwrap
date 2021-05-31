cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD "20")
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project("imguiwrap")

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
	set (IMGW_NO_EXCEPTIONS "/EHc-")
	set (IMGW_ALL_WARNINGS  "/W3")
	set (IMGW_NO_RTTI "/GR-")
else ()
	set (IMGW_NO_EXCEPTIONS "-fno-exceptions")
	set (IMGW_ALL_WARNINGS  "-Wall -Werror")
	set (IMGW_NO_RTTI "-fno-rtti")
endif ()

target_compile_options(imguiwrap PRIVATE ${IMGW_NO_RTTI} ${IMGW_NO_EXCEPTIONS} ${IMGW_ALL_WARNINGS})

# Are we standalone?
if ("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_SOURCE_DIR}")
	set(IMGUIWRAP_STANDALONE TRUE)
endif()

if (IMGUIWRAP_STANDALONE)
	add_subdirectory(example)
endif ()
