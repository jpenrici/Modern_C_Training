cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

project(
  TicTacToe
  VERSION 1.0.0
  LANGUAGES C)

set(PROGRAM_NAME game)
set(LIBRARY_NAME engine)
set(TEST_NAME test_engine)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF) # Use -std=c23, not -std=gnu23.

include_directories(include)
add_library(${LIBRARY_NAME} STATIC src/engine.c)

add_executable(${PROGRAM_NAME} main.c)
target_link_libraries(${PROGRAM_NAME} ${LIBRARY_NAME})

add_executable(${TEST_NAME} test/test.c)
target_link_libraries(${TEST_NAME} PRIVATE ${LIBRARY_NAME})

# CTest integration: cmake --build build && ctest --test-dir build
enable_testing()
add_test(NAME engine_tests COMMAND ${TEST_NAME})

include(GNUInstallDirs) # Defines CMAKE_INSTALL_BINDIR / LIBDIR correctly.

install(
  TARGETS ${PROGRAM_NAME}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
