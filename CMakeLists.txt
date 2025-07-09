cmake_minimum_required(VERSION 3.30)

project(optica)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(optica INTERFACE)
add_library(optica::optica ALIAS optica)

target_sources(optica INTERFACE FILE_SET HEADERS
BASE_DIRS
  include
FILES
  include/optica/optica.hpp
  include/optica/impl/fixed_string.hpp
  include/optica/impl/properties.hpp
  include/optica/impl/program_option.hpp
  include/optica/impl/parser.hpp
  include/optica/impl/type_parsers.hpp
)

add_subdirectory(examples)
