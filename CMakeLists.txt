cmake_minimum_required(VERSION 3.23)
set(CMAKE_CXX_STANDARD 23)

project(MarkdownParser)
add_executable(parser main.cpp)

# These tests can use the Catch2-provided main
find_package(Catch2 2 REQUIRED)
add_executable(tests tests.cpp)
