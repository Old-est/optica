add_executable(test1)
target_sources(test1 PRIVATE test.cpp)
target_link_libraries(test1 PRIVATE optica::optica)
