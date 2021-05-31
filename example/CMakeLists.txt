cmake_minimum_required(VERSION 3.16)

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

target_compile_options(basic_example PRIVATE ${IMGW_NO_EXCEPTIONS} ${IMGW_ALL_WARNINGS})

add_executable(
	dear_example
	dear_example.cpp
)

target_link_libraries(
	dear_example
	PUBLIC
	imguiwrap
)

target_compile_options(dear_example PRIVATE ${IMGW_NO_EXCEPTIONS} ${IMGW_ALL_WARNINGS})
