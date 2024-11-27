set(ENV{INCLUDE_DIRS} "$ENV{INCLUDE_DIRS};${CMAKE_CURRENT_SOURCE_DIR}")

add_subdirectory(store)
add_subdirectory(lexer)
add_subdirectory(parser)
add_subdirectory(argument)

set(INCLUDE_DIRS_LIST $ENV{INCLUDE_DIRS})
string(REPLACE ";" ";" INCLUDE_DIRS_LIST "${INCLUDE_DIRS_LIST}")

add_library(arg_parser arg_parser.cpp)
target_include_directories(arg_parser PRIVATE ${INCLUDE_DIRS_LIST})
target_link_libraries(arg_parser PRIVATE
  parser
  lexer
  argument
)