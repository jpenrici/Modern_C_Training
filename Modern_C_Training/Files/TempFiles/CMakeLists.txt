cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

project(
  TempFiles
  VERSION 1.0.0
  LANGUAGES C)

set(PROGRAM_NAME test)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(${PROGRAM_NAME} src/main.c)
