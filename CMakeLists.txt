cmake_minimum_required(VERSION 3.22)
project(Airport)

set(CMAKE_CXX_STANDARD 14)

add_executable(Airport main.cpp)

configure_file(${CMAKE_SOURCE_DIR}/airlines.csv ${CMAKE_BINARY_DIR}/airlines.csv COPYONLY)
