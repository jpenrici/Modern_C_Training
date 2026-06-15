cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

project(
  Project
  VERSION 1.0.0
  DESCRIPTION "Word counter using C23 + embedded Perl (libperl)"
  LANGUAGES C)

set(PROGRAM_NAME word_counter)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON) # Perl needs extensions

find_package(Perl REQUIRED)
find_package(PerlLibs REQUIRED)

add_executable(${PROGRAM_NAME} src/main.c)
target_include_directories(${PROGRAM_NAME} PRIVATE ${PERL_INCLUDE_DIRS})
target_compile_options(${PROGRAM_NAME} PRIVATE -Wall -Wextra)
target_link_libraries(${PROGRAM_NAME} PRIVATE ${PERL_LIBRARIES})

# Configures RPATH automatically to find libperl at runtime
set_target_properties(${PROGRAM_NAME} PROPERTIES BUILD_WITH_INSTALL_RPATH TRUE)
target_link_options(${PROGRAM_NAME} PRIVATE -Wl,-rpath,${PERL_LIBRARY_DIR})

add_custom_command(
  TARGET ${PROGRAM_NAME}
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory
          $<TARGET_FILE_DIR:${PROGRAM_NAME}>/perl
  COMMAND ${CMAKE_COMMAND} -E make_directory
          $<TARGET_FILE_DIR:${PROGRAM_NAME}>/resource
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/perl/counter.pl
          $<TARGET_FILE_DIR:${PROGRAM_NAME}>/perl/
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/resource/sample.txt
          $<TARGET_FILE_DIR:${PROGRAM_NAME}>/resource/
  COMMENT "Copying Perl script and sample.txt to build output")

# Status de Build
message(STATUS "")
message(STATUS "  ${PROGRAM_NAME} — build summary")
message(STATUS "  C standard   : C23 (extensions ON for libperl)")
message(STATUS "  Perl binary  : ${PERL_EXECUTABLE}")
message(STATUS "  Perl headers : ${PERL_INCLUDE_DIRS}")
message(STATUS "  Perl libs    : ${PERL_LIBRARIES}")
message(STATUS "")
