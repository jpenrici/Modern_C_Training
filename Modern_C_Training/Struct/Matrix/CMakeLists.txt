cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

project(
  Matrix
  VERSION 1.0.0
  LANGUAGES C)

set(PROGRAM_NAME calc)
set(LIBRARY_NAME matrix)

set(TEST_LIB_NAME matrix_test)
set(TEST_NAME test)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF) # Use -std=c23, not -std=gnu23.

include_directories(include)
add_library(${LIBRARY_NAME} STATIC src/matrix.c)

add_library(${TEST_LIB_NAME} STATIC src/matrix.c)
target_compile_definitions(${TEST_LIB_NAME} PRIVATE UNIT_TESTING)

add_executable(${PROGRAM_NAME} main.c)
target_link_libraries(${PROGRAM_NAME} ${LIBRARY_NAME})

add_executable(${TEST_NAME} test/test.c)
target_compile_definitions(${TEST_NAME} PRIVATE UNIT_TESTING)
target_link_libraries(${TEST_NAME} PRIVATE ${TEST_LIB_NAME})

install(
  TARGETS ${PROGRAM_NAME}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
