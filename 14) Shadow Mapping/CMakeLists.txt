set(SOURCES
    tutorial14.cpp
    Renderer.cpp
)

set(HEADERS
    Renderer.h
)

add_executable(tutor14 ${SOURCES} ${HEADERS})
target_link_libraries(tutor14 PRIVATE nclgl)