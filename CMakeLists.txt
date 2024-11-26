cmake_minimum_required(VERSION 3.12)

project(
    labwork4
    VERSION 1.0
    DESCRIPTION "C++ Year 2024 labword4"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_definitions(LABA4)

# directory envvar
set(ENV{INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(lib)
add_subdirectory(bin)

enable_testing()
add_subdirectory(tests)

set(ENV{INCLUDE_DIRS} "")
set(ENV{LIB_DIR} "")