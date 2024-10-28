set(SOURCES
    Blur.cpp
    Camera.cpp
    ComputeShader.cpp
    CubeBot.cpp
    Frustum.cpp
    GameTimer.cpp
    HeightMap.cpp
    Keyboard.cpp
    Matrix2.cpp
    Matrix3.cpp
    Matrix4.cpp
    Mesh.cpp
    MeshAnimation.cpp
    MeshMaterial.cpp
    # Head in the sand approach to removing this file
    # Mouse.cpp
    OGLRenderer.cpp
    Quaternion.cpp
    ResourceManager.cpp
    SceneNode.cpp
    Shader.cpp
    SkeletonAnim.cpp
    Stage.cpp
    # Oh look, something else to ignore
    # Window.cpp
)

find_package(OpenGL REQUIRED)

add_library(nclgl STATIC ${SOURCES})
target_include_directories(nclgl PRIVATE "${PROJECT_SOURCE_DIR}/Third Party")
