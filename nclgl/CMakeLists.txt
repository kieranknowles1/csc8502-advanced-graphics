set(SOURCES
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
    Window.cpp
    # TODO: We should probably be using a library for this
    "${PROJECT_SOURCE_DIR}/Third Party/glad/glad.c"

    PostProcess/Blur.cpp
    PostProcess/Stage.cpp
)

set(HEADERS
    Camera.h
    common.h
    ComputeShader.h
    CubeBot.h
    Frustum.h
    GameTimer.h
    HeightMap.h
    InputDevice.h
    Keyboard.h
    Light.h
    Matrix2.h
    Matrix3.h
    Matrix4.h
    Mesh.h
    MeshAnimation.h
    MeshMaterial.h
    Mouse.h
    OGLRenderer.h
    Plane.h
    Quaternion.h
    ResourceManager.h
    SceneNode.h
    Shader.h
    SkeletonAnim.h
    Vector2.h
    Vector3.h
    Vector4.h
    Window.h

    PostProcess/Blur.h
    PostProcess/Stage.h
)

find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)

add_library(nclgl ${SOURCES} ${HEADERS})
target_include_directories(nclgl PUBLIC "${PROJECT_SOURCE_DIR}/Third Party")

target_link_libraries(nclgl INTERFACE
    ${OPENGL_LIBRARIES}
    SDL2::SDL2
    SOIL
)
