file(GLOB_RECURSE ALL_SOURCE_FILES CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/include/*.hpp" "${CMAKE_SOURCE_DIR}/mod/*.cppm"
     "${CMAKE_SOURCE_DIR}/tests/*.cpp")

add_custom_target(clang-format-check)

add_custom_target(
  clang_format_check
  COMMAND clang-format --dry-run --Werror ${ALL_SOURCE_FILES}
  COMMENT "Checking code style with clang-format"
  VERBATIM)

add_custom_target(
  clang_format_fix
  COMMAND clang-format -i ${ALL_SOURCE_FILES}
  COMMENT "Auto-formatting code with clang-format"
  VERBATIM)
