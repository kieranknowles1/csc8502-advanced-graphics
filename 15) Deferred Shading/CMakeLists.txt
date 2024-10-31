set(SOURCES
    Tutorial15.cpp
    Renderer.cpp
)

set(HEADERS
    Renderer.h
)

add_executable(tutor15 ${SOURCES} ${HEADERS})
target_link_libraries(tutor15 nclgl)