cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

project(
  FunctionsPtr
  VERSION 1.0.0
  LANGUAGES C)

set(PROGRAM_NAME test)
set(LIBRARY_NAME calculate)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

include_directories(include)
add_library(${LIBRARY_NAME} STATIC src/calc.c)

set(SOURCES ./src/main.c)

add_executable(${PROGRAM_NAME} ${SOURCES})
target_link_libraries(${PROGRAM_NAME} PRIVATE ${LIBRARY_NAME})
