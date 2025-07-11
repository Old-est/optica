list(APPEND CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)

file(GLOB TEST_SOURCES CONFIGURE_DEPENDS *.cpp)

add_custom_target(optica-tests)

foreach(SOURCE IN LISTS TEST_SOURCES)
    message(STATUS "Collecting Test: ${SOURCE}")
    get_filename_component(TEST "${SOURCE}" NAME_WE)
    add_executable(${TEST})
    target_sources(${TEST} PRIVATE ${SOURCE})
    target_link_libraries(${TEST} PRIVATE optica::optica Catch2::Catch2WithMain)
    add_dependencies(optica-tests ${TEST})
    catch_discover_tests(${TEST} REPORTER xml)
endforeach()

add_custom_target(run-tests ALL
     COMMAND ${CMAKE_COMMAND} -E env CLICOLOR_FORCE=1 ${CMAKE_CTEST_COMMAND}
    DEPENDS optica-tests
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running all tests..."
)
