set(SOURCES
    Renderer.cpp
    TimeWarp.cpp
    main.cpp
)

set(HEADERS
    Renderer.h
    TimeWarp.h
)

add_executable(coursework ${SOURCES} ${HEADERS})
target_link_libraries(coursework nclgl)
