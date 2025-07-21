cmake_minimum_required(VERSION 3.30)

project(optica)
if(NOT CMAKE_COLOR_DIAGNOSTICS)
  set(CMAKE_COLOR_DIAGNOSTICS ON)
endif()
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(cmake/deps.cmake)
include(cmake/clang-format.cmake)

option(OPTICA_MODULE "Build C++ module" OFF)

if(OPTICA_MODULE)
  add_library(optica)
  target_sources(optica PUBLIC FILE_SET CXX_MODULES TYPE CXX_MODULES FILES
                               "${CMAKE_SOURCE_DIR}/mod/optica.cppm")
  target_sources(
    optica
    PUBLIC FILE_SET
           HEADERS
           TYPE
           HEADERS
           BASE_DIRS
           "${CMAKE_SOURCE_DIR}/include"
           FILES
           include/optica/optica.hpp
           include/optica/impl/fixed_string.hpp
           include/optica/impl/properties.hpp
           include/optica/impl/option_builder.hpp
           include/optica/impl/option.hpp
           include/optica/impl/token.hpp
           include/optica/impl/parser.hpp
           include/optica/impl/type_parsers.hpp)
else()
  add_library(optica INTERFACE)

  target_sources(
    optica
    INTERFACE FILE_SET
              HEADERS
              TYPE
              HEADERS
              BASE_DIRS
              "${CMAKE_SOURCE_DIR}/include"
              FILES
              include/optica/optica.hpp
              include/optica/impl/fixed_string.hpp
              include/optica/impl/properties.hpp
              include/optica/impl/option_builder.hpp
              include/optica/impl/option.hpp
              include/optica/impl/token.hpp
              include/optica/impl/parser.hpp
              include/optica/impl/type_parsers.hpp)
endif()

add_library(optica::optica ALIAS optica)

add_subdirectory(examples)

option(OPTICA_BUILD_TESTS "Build tests" OFF)

if(OPTICA_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()
