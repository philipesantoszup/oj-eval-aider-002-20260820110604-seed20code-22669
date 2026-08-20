cmake_minimum_required(VERSION 3.10)
project(int2048)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(src/include)

add_library(int2048 STATIC src/int2048.cpp)

add_executable(test test.cpp)
target_link_libraries(test int2048)
