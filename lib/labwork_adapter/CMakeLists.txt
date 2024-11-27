set(ENV{INCLUDE_DIRS} "$ENV{INCLUDE_DIRS};${CMAKE_CURRENT_SOURCE_DIR}")

set(INCLUDE_DIRS_LIST $ENV{INCLUDE_DIRS})
string(REPLACE ";" ";" INCLUDE_DIRS_LIST "${INCLUDE_DIRS_LIST}")

add_library(labwork_adapter STATIC ArgParser.cpp)

target_include_directories(labwork_adapter PRIVATE ${INCLUDE_DIRS_LIST})
target_link_libraries(labwork_adapter PRIVATE 
  arg_parser
  argument
  store
)