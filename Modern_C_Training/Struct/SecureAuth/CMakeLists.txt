cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

project(
  ModernC_Training
  VERSION 1.0.0
  LANGUAGES C)

set(PROGRAM_NAME auth_demo)
set(LIB_NAME secure_auth)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

include_directories(include)
add_library(${LIB_NAME} STATIC src/secure_auth.c)

add_executable(${PROGRAM_NAME} src/main.c)
target_link_libraries(${PROGRAM_NAME} ${LIB_NAME})

install(
  TARGETS ${PROGRAM_NAME}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
