list(APPEND CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)

file(GLOB TEST_SOURCES CONFIGURE_DEPENDS *.cpp)

add_custom_target(optica-tests)

foreach(SOURCE IN LISTS TEST_SOURCES)
  message(STATUS "Collecting Test: ${SOURCE}")
  get_filename_component(TEST "${SOURCE}" NAME_WE)
  add_executable(${TEST}_test)
  target_sources(${TEST}_test PRIVATE ${SOURCE})
  target_link_libraries(${TEST}_test PRIVATE optica::optica
                                             Catch2::Catch2WithMain)
  add_dependencies(optica-tests ${TEST}_test)
  catch_discover_tests(${TEST}_test REPORTER xml)
endforeach()

