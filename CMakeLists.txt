cmake_minimum_required(VERSION 3.10)
project(multiple_isosuface C)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

set(CMAKE_C_STANDARD 99)

add_subdirectory(sample)
add_subdirectory(src)
