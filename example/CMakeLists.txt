cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD "${IMGUIWRAP_CXX_STANDARD}")
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Simple raw example of how to link against imguiwrap.
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

macro(add_imguiwrap_example eg_name)
	add_executable(${eg_name} ${eg_name}.cpp)
	target_link_libraries(${eg_name} PUBLIC imguiwrap)
	target_compile_options(${eg_name} PRIVATE ${IMGW_NO_EXCEPTIONS} ${IMGW_ALL_WARNINGS})
endmacro()

add_imguiwrap_example(dear_example)
add_imguiwrap_example(edit_window_example)
add_imguiwrap_example(dear_example2)
