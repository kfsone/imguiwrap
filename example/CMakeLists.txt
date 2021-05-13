cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD "20")
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(
	basic_example
	basic_example.cpp
)

target_link_libraries(
	basic_example
	PUBLIC
	imguiwrap
)

add_executable(
	dear_example
	dear_example.cpp
)

target_link_libraries(
	dear_example
	PUBLIC
	imguiwrap
)

