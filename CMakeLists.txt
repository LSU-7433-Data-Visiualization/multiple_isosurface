cmake_minimum_required(VERSION 3.10)
project(multiple_isosuface C)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

set(CMAKE_C_STANDARD 99)
add_executable(main main.c)

include_directories(${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS})
target_link_libraries(main ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} m)

