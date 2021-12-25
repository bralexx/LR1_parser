cmake_minimum_required(VERSION 3.16)
project(LR1_parser)

set(CMAKE_CXX_STANDARD 17)
set(EXTERNAL_INSTALL_LOCATION ${CMAKE_BINARY_DIR}/gtest_repo)
add_executable(LR1_parser main.cpp Grammar.cpp)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)

FetchContent_MakeAvailable(googletest)


add_executable(
        LR1_parser_tests
        tests/test1.cpp
        tests/test_main.cpp
        tests/test_functions.cpp
        Grammar.cpp
        tests/test2.cpp
        tests/test3.cpp)
target_link_libraries(
        LR1_parser_tests
        gtest_main
)