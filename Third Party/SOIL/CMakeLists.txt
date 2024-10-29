set(SOURCES
    image_DXT.c
    image_helper.c
    SOIL.c
    stb_image_aug.c
)

add_library(SOIL ${SOURCES})
target_include_directories(SOIL PRIVATE "${PROJECT_SOURCE_DIR}/Third Party")
