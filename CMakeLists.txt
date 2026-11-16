cmake_minimum_required(VERSION 3.16)

project(catch_practice LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.8.1
)

FetchContent_MakeAvailable(Catch2)

add_executable(tests practice_temp/test_math.cpp)
add_executable(ga_tests tests/ga_tests.cpp)
add_executable(scheduling heterogeneous_machines/scheduling.cpp)
add_executable(scheduling_tests tests/scheduling_tests.cpp)
add_executable(topological_sort_tests tests/topological_sort_tests.cpp)

target_include_directories(scheduling PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(scheduling_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)
target_link_libraries(ga_tests PRIVATE Catch2::Catch2WithMain)
target_link_libraries(scheduling_tests PRIVATE Catch2::Catch2WithMain)

enable_testing()
add_test(NAME ga_tests COMMAND ga_tests)
add_test(NAME scheduling_tests COMMAND scheduling_tests)
add_test(NAME topological_sort_tests COMMAND topological_sort_tests)
