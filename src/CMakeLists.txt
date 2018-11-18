cmake_minimum_required(VERSION 3.10)
project(multiple_isosuface C)

add_executable(main main.c)

configure_file(DataDensity DataDensity COPYONLY)

include_directories(${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(main ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} m)
