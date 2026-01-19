cmake_minimum_required(VERSION 3.23)
set(CMAKE_CXX_STANDARD 23)

project(MarkdownParser)
add_executable(exec main.cpp)
target_link_libraries(exec
    PRIVATE
        types
        parser
)

# These tests can use the Catch2-provided main
find_package(Catch2 2 REQUIRED)
add_executable(tests tests.cpp)
target_link_libraries(tests
    PRIVATE
        types
        parser
)

add_library(types)
target_sources(types
    PRIVATE
        ./types.cpp

    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
            ./
        FILES
            ./types.h
)

add_library(parser)
target_sources(parser
    PRIVATE
        ./parser.cpp

    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
            ./
        FILES
            ./parser.h
)
