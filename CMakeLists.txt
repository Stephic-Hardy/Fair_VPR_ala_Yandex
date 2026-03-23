cmake_minimum_required(VERSION 3.10)
project(app VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(UTILS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/utils")
set(UTILS_SOURCES
        "${UTILS_DIR}/json_parser.cpp"
        "${UTILS_DIR}/problem_arguments.cpp"
)

set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(SRC_SOURCES
        "${SRC_DIR}/first_step.cpp"
        "${SRC_DIR}/branch_and_bound.cpp"
        "${SRC_DIR}/multi_agent_solver.cpp"
        "${SRC_DIR}/single_agent_solver.cpp"
)

add_library(utils STATIC ${UTILS_SOURCES})
target_include_directories(utils PUBLIC utils)

add_library(algo STATIC ${SRC_SOURCES})
target_include_directories(algo PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})   # добавлено
target_link_libraries(algo PRIVATE utils)                              # добавлено

set(SOURCES main.cpp)

add_executable(app ${SOURCES})
target_link_libraries(app PRIVATE utils algo)

#target_compile_definitions(app PRIVATE DEBUG)

set(TESTS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tests")
enable_testing()
add_subdirectory(${TESTS_DIR})

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DDEBUG)
endif()