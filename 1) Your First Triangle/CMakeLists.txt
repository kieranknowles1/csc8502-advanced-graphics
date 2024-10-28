set(SOURCES
    Renderer.cpp
    tutorial1.cpp
)

add_executable(tutorial1 ${SOURCES})
target_include_directories(tutorial1 PRIVATE "${PROJECT_SOURCE_DIR}")
target_include_directories(tutorial1 PRIVATE "${PROJECT_SOURCE_DIR}/Third Party")

target_link_libraries(tutorial1 PRIVATE nclgl)
