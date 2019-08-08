# Specify the minimum version of CMAKE

cmake_minimum_required(VERSION 3.13.4)

# Projects Name
project(weatherStationPlus)

#Output folders
set(CMAKE_BINARY_DIR bin)
set(CMAKE_SOURCE_DIR src)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)