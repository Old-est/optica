cmake_minimum_required(VERSION 3.30)

project(optica)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(optica INTERFACE)
add_library(optica::optica ALIAS optica)

target_sources(optica INTERFACE FILE_SET HEADERS BASE_DIRS include
FILES include/optica/optica.hpp)

add_subdirectory(examples)
