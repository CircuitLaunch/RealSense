cmake_minimum_required(VERSION 3.10)

project(realsense_test VERSION 0.0.001)

set(OpenGL_GL_PREFERENCE GLVND)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(OpenGL REQUIRED)
find_package(PCL REQUIRED)

add_executable(${PROJECT_NAME}
  main.cpp
  Window.hpp Window.cpp
  Texture.hpp Texture.cpp)

target_link_libraries(${PROJECT_NAME} realsense2 -lglfw OpenGL::GL OpenGL::GLU ${PCL_LIBRARIES})
