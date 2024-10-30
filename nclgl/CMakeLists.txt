set(SOURCES
    Blur.cpp
    Camera.cpp
    ComputeShader.cpp
    CubeBot.cpp
    Frustum.cpp
    GameTimer.cpp
    HeightMap.cpp
    Keyboard.cpp
    Light.cpp
    Matrix2.cpp
    Matrix3.cpp
    Matrix4.cpp
    MeshAnimation.cpp
    Mesh.cpp
    MeshMaterial.cpp
    Mouse.cpp
    OGLRenderer.cpp
    Plane.cpp
    Quaternion.cpp
    ResourceManager.cpp
    SceneNode.cpp
    Shader.cpp
    SkeletonAnim.cpp
    Stage.cpp
    Window.cpp
    # TODO: We should probably be using a library for this
    "${PROJECT_SOURCE_DIR}/Third Party/glad/glad.c"
)

find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)

add_library(nclgl ${SOURCES})
target_include_directories(nclgl PUBLIC "${PROJECT_SOURCE_DIR}/Third Party")

target_link_libraries(nclgl INTERFACE
    ${OPENGL_LIBRARIES}
    SDL2::SDL2
    SOIL
)
