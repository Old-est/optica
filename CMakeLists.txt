cmake_minimum_required(VERSION 3.30)

project(optica)
if(NOT CMAKE_COLOR_DIAGNOSTICS)
    set(CMAKE_COLOR_DIAGNOSTICS ON)
endif()
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(cmake/deps.cmake)

add_library(optica INTERFACE)
add_library(optica::optica ALIAS optica)

target_sources(optica INTERFACE FILE_SET HEADERS
BASE_DIRS
  include
FILES
  include/optica/optica.hpp
  include/optica/impl/utils.hpp
  include/optica/impl/fixed_string.hpp
  include/optica/impl/properties.hpp
  include/optica/impl/program_option_builder.hpp
  include/optica/impl/program_option.hpp
  include/optica/impl/parser.hpp
  include/optica/impl/type_parsers.hpp
)

add_subdirectory(examples)

option(OPTICA_BUILD_TESTS "Build tests" OFF)

if(OPTICA_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

