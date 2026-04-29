cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

project(
  TicTacToe
  VERSION 1.0.0
  LANGUAGES C)

set(PROGRAM_NAME game)
set(LIBRARY_NAME engine)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(include)
add_library(${LIBRARY_NAME} STATIC src/engine.c)

add_executable(${PROGRAM_NAME} main.c)
target_link_libraries(${PROGRAM_NAME} ${LIBRARY_NAME})

install(
  TARGETS ${PROGRAM_NAME}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
