cmake_minimum_required(VERSION 3.15)
project(python)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(lib)
add_subdirectory(bin)
add_subdirectory(tests)

enable_testing()
