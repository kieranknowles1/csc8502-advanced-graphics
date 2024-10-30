set(SOURCES
    Tutorial13.cpp
    Renderer.cpp
)

set(HEADERS
    Renderer.h
)

add_executable(tutor13 ${SOURCES} ${HEADERS})
target_link_libraries(tutor13 PRIVATE nclgl)