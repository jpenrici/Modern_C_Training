cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

project(
  JuliaSetPng
  VERSION 1.0.0
  LANGUAGES C)

set(PROGRAM_NAME test)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(SOURCES ./src/main.c)

add_executable(${PROGRAM_NAME} ${SOURCES})

find_package(PNG REQUIRED)

target_link_libraries(${PROGRAM_NAME} PRIVATE PNG::PNG m)

target_compile_options(${PROGRAM_NAME} PRIVATE -Wall -Wextra -Wpedantic)

install(
  TARGETS ${PROGRAM_NAME}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
