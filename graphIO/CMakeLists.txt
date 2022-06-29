cmake_minimum_required(VERSION 3.12...3.18)

project(${IO_LIBRARY})

set(SOURCE_EXE main.cpp)

add_library(${IO_LIBRARY} STATIC
	src/Vectorgraph.cpp
	include/Vectorgraph.h )

target_include_directories(${IO_LIBRARY} PUBLIC include)

add_executable(IOmain ${SOURCE_EXE})

target_link_libraries(IOmain PUBLIC ${IO_LIBRARY})