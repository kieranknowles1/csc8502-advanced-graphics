set(SOURCES
    Tutorial12.cpp
    Renderer.cpp
)

set(HEADERS
    Renderer.h
)

add_executable(tutor12 ${SOURCES} ${HEADERS})
target_link_libraries(tutor12 nclgl)