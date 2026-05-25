cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

project(
  FinancialCRUD
  VERSION 1.0.0
  LANGUAGES C)

set(PROGRAM_NAME test)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(SOURCES ./src/main.c)

find_package(SQLite3 REQUIRED)

add_executable(${PROGRAM_NAME} ${SOURCES})
target_include_directories(${PROGRAM_NAME} PRIVATE ${SQLite3_INCLUDE_DIRS})
target_link_libraries(${PROGRAM_NAME} PRIVATE SQLite::SQLite3)

add_compile_options(-Wall -Wextra -Wpedantic)
