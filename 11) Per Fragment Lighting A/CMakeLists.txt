set(SOURCES
    Tutorial11.cpp
    Renderer.cpp
)

add_executable(tutor11 ${SOURCES})

target_link_libraries(tutor11 nclgl)
