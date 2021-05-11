cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD "20")
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(
	simple_example

	main.cpp
)

target_link_libraries(
	simple_example

	PUBLIC

	imguiwrap
)
