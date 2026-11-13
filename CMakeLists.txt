cmake_minimum_required(VERSION 3.16)

project(catch_practice LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.8.1
)

FetchContent_MakeAvailable(Catch2)

add_executable(tests practice_temp/test_math.cpp)
add_executable(ga_tests tests/ga_tests.cpp)

target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)
target_link_libraries(ga_tests PRIVATE Catch2::Catch2WithMain)