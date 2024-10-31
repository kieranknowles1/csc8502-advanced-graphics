set(SOURCES
    Renderer.cpp
    Tutorial10.cpp
)

set(HEADERS
    Renderer.h
)

add_executable(tutor10 ${SOURCES} ${HEADERS})
target_link_libraries(tutor10 nclgl)
