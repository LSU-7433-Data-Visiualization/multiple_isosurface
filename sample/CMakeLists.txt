cmake_minimum_required(VERSION 3.10)
project(sample C)

add_executable(MC MC.c isosurface.h)

configure_file(ChargeDensity ChargeDensity COPYONLY)

include_directories(${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(MC ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} m)
