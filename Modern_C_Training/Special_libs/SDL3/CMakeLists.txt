cmake_minimum_required(VERSION 3.25 FATAL_ERROR)

project(
  HelloSDL
  VERSION 1.0.0
  LANGUAGES C)

set(PROGRAM_NAME test)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

find_package(SDL3 REQUIRED)

set(SOURCES ./src/main.c)

add_executable(${PROGRAM_NAME} ${SOURCES})

target_link_libraries(${PROGRAM_NAME} PRIVATE SDL3::SDL3 m) # libm

add_compile_options(-Wall -Wextra -Wpedantic)

install(
  TARGETS ${PROGRAM_NAME}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
