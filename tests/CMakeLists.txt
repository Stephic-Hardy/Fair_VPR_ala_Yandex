cmake_minimum_required(VERSION 3.10)

add_subdirectory(${CMAKE_SOURCE_DIR}/external/googletest googletest-build)

set(TEST_SOURCES test_json.cpp test_first_step.cpp)
set(SOURCES ${TEST_SOURCES})

add_executable(tests ${SOURCES})
target_compile_definitions(tests PRIVATE DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data")

target_link_libraries(tests algo utils GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(tests)
