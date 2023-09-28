cmake_minimum_required(VERSION 3.5)

project(Particlesystem LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})
link_libraries(${GLEW_LIBRARIES})
find_package(glfw3 REQUIRED)

add_executable(Particlesystem main.cpp particlesystem.h shaders.hpp glerror.hpp)
target_link_libraries(Particlesystem ${OPENGL_gl_LIBRARY} glfw)
#install(TARGETS Particlesystem
#    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
#    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
#)
