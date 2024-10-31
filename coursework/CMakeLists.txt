set(SOURCES
    Renderer.cpp
    main.cpp
)

set(HEADERS
    Renderer.h
)

add_executable(coursework ${SOURCES} ${HEADERS})
target_link_libraries(coursework nclgl)
