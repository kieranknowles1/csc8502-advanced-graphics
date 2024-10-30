set(SOURCES
    Tutorial11.cpp
    Renderer.cpp
)

set(HEADERS
    Renderer.h
)

add_executable(tutor11 ${SOURCES} ${HEADERS})

target_link_libraries(tutor11 nclgl)
install(TARGETS tutor11)
