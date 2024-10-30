cmake_minimum_required(VERSION 3.29)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(AdvancedGraphics)

add_subdirectory(nclgl)
add_subdirectory("Third Party/SOIL/Simple OpenGL Image Library/src")

add_subdirectory("11) Per Fragment Lighting A")
