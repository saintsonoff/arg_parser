set(INCLUDE_DIRS_LIST $ENV{INCLUDE_DIRS})
string(REPLACE ";" ";" INCLUDE_DIRS_LIST "${INCLUDE_DIRS_LIST}")

add_executable(${PROJECT_NAME} main.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIRS_LIST})
target_link_libraries(${PROJECT_NAME} PRIVATE 
  arg_parser
  argument
  store
  labwork_adapter
)