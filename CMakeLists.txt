cmake_minimum_required(VERSION 3.28)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

project(AdvancedGraphics)

# Workaround to build on windows
if(WIN32)
	list(APPEND CMAKE_PREFIX_PATH H:/cmake)
endif(WIN32)

add_subdirectory(nclgl)
add_subdirectory("Third Party/SOIL/Simple OpenGL Image Library/src")

add_subdirectory("10) Post Processing")
add_subdirectory("11) Per Fragment Lighting A")
add_subdirectory("12) Per Fragment Lighting B")
add_subdirectory("13) Cube Mapping")
add_subdirectory("14) Shadow Mapping")
add_subdirectory("15) Deferred Shading")
add_subdirectory("coursework")
