cmake_minimum_required(VERSION 3.12...3.18)

set(PROJECT_NAME graphAlgorithms)

project(${PROJECT_NAME})

set(IO_LIBRARY graphIO)
set(MST_LIBRARY MST)

find_package(OpenMP)

#subprojects
add_subdirectory(graphIO)
add_subdirectory(MST)