cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

project(
  Matrix
  VERSION 1.0.0
  LANGUAGES C)

set(PROGRAM_NAME calc)
set(LIBRARY_NAME matrix)
set(TEST_NAME test)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF) # Use -std=c23, not -std=gnu23.

include_directories(include)
add_library(${LIBRARY_NAME} STATIC src/matrix.c)

add_executable(${PROGRAM_NAME} main.c)
target_link_libraries(${PROGRAM_NAME} ${LIBRARY_NAME})

add_executable(${TEST_NAME} test/test.c)
target_link_libraries(${TEST_NAME} PRIVATE ${LIBRARY_NAME})

install(
  TARGETS ${PROGRAM_NAME}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
