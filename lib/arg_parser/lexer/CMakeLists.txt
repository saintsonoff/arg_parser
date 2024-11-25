set(ENV{INCLUDE_DIRS} "$ENV{INCLUDE_DIRS};${CMAKE_CURRENT_SOURCE_DIR}")

set(INCLUDE_DIRS_LIST $ENV{INCLUDE_DIRS})
string(REPLACE ";" ";" INCLUDE_DIRS_LIST "${INCLUDE_DIRS_LIST}")

# message("\n\n\n\n\n\n\n")
# message(INCLUDE_DIRECTORIES)
# message("\n\n\n\n\n\n\n")

add_library(lexer lexer.cpp)
target_include_directories(lexer PRIVATE ${INCLUDE_DIRS_LIST})