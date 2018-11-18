cmake_minimum_required(VERSION 3.10)
project(multiple_isosuface C)

add_executable(main main.c)

include_directories(${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS})
target_link_libraries(main ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} m)
