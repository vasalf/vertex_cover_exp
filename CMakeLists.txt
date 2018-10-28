cmake_minimum_required(VERSION 3.8)
project(vertex_cover_exp)

set(CMAKE_CXX_STANDARD 17)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Copied from github.com/vasalf/algorithm_test
add_subdirectory(ascii_table)

add_subdirectory(lib)
add_subdirectory(kernels)
