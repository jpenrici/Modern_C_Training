cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

project(
  Geometry
  VERSION 1.0.0
  LANGUAGES C)

set(PROGRAM_NAME geometry)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(SOURCES src/main.c src/shape.c src/circle.c src/rectangle.c)

add_executable(${PROGRAM_NAME} ${SOURCES})
target_include_directories(${PROGRAM_NAME}
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_compile_options(-Wall -Wextra -Wpedantic)
