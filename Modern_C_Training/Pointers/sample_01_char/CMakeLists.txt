cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

project(
  PointersTraining
  VERSION 1.0.0
  LANGUAGES C)

set(PROGRAM_NAME test)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(SOURCES ./src/main.c)

add_executable(${PROGRAM_NAME} ${SOURCES})
