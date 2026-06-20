cmake_minimum_required(VERSION 3.25 FATAL_ERROR)

project(
  HelloWorld
  VERSION 1.0.0
  LANGUAGES C)

set(PROGRAM_NAME helloWorld)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

include_directories(./src/external/glad/include)

find_package(PkgConfig REQUIRED)
find_package(OpenGL REQUIRED)

pkg_check_modules(GLFW REQUIRED glfw3)

set(SOURCES ./src/main.c ./src/external/glad/src/glad.c)

add_executable(${PROGRAM_NAME} ${SOURCES})

target_link_libraries(${PROGRAM_NAME} PRIVATE OpenGL::GL ${GLFW_LIBRARIES})

add_compile_options(-Wall -Wextra -Wpedantic)
