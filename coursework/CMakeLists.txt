set(SOURCES
    CameraPath.cpp
    Renderer.cpp
    TimeWarp.cpp
    main.cpp
)

set(HEADERS
    CameraPath.h
    Renderer.h
    TimeWarp.h
)

# Files to copy to build directory
# Not the cleanest way to do this
set(RESOURCES
    "Textures/Barren Reds.JPG"
    "Textures/Barren RedsDOT3.JPG"
    Meshes/quaternius/DeadTree_1.mat
    Meshes/quaternius/DeadTree_1.msh
    Meshes/quaternius/DeadTree_2.mat
    Meshes/quaternius/DeadTree_2.msh
    Meshes/quaternius/DeadTree_3.mat
    Meshes/quaternius/DeadTree_3.msh
    Meshes/quaternius/DeadTree_4.mat
    Meshes/quaternius/DeadTree_4.msh
    Meshes/quaternius/DeadTree_5.mat
    Meshes/quaternius/DeadTree_5.msh
    Meshes/quaternius/Pine_1.mat
    Meshes/quaternius/Pine_1.msh
    Meshes/quaternius/Pine_2.mat
    Meshes/quaternius/Pine_2.msh
    Meshes/quaternius/Pine_3.mat
    Meshes/quaternius/Pine_3.msh
    Meshes/quaternius/Pine_4.mat
    Meshes/quaternius/Pine_4.msh
    Meshes/quaternius/Pine_5.mat
    Meshes/quaternius/Pine_5.msh
    Meshes/quaternius/Rock_Medium_1.mat
    Meshes/quaternius/Rock_Medium_1.msh
    Meshes/quaternius/Rock_Medium_2.mat
    Meshes/quaternius/Rock_Medium_2.msh
    Meshes/quaternius/Rock_Medium_3.mat
    Meshes/quaternius/Rock_Medium_3.msh
    Meshes/Role_T.anm
    Meshes/Role_T.mat
    Meshes/Role_T.msh
    Meshes/Sphere.msh
    Shaders/BufferFragment.glsl
    Shaders/BumpVertex.glsl
    Shaders/CombineFrag.glsl
    Shaders/CombineVert.glsl
    Shaders/DeferredLight.frag
    Shaders/DeferredLight.vert
    Shaders/post/passthroughVertex.glsl
    Shaders/post/timeWarp.glsl
    Shaders/ShadowFrag.glsl
    Shaders/ShadowVert.glsl
    Shaders/SkinningVertex.glsl
    Shaders/SkinningVertexShadow.vert
    Shaders/SkyboxFragment.glsl
    Shaders/SkyboxVertex.glsl
    Shaders/TessControl.glsl
    Shaders/TessControlShadow.glsl
    Shaders/TessEval.glsl
    Shaders/TessEvalShadow.glsl
    Shaders/TessVertex.glsl
    Textures/crystallotus/blue_nx.png
    Textures/crystallotus/blue_ny.png
    Textures/crystallotus/blue_nz.png
    Textures/crystallotus/blue_px.png
    Textures/crystallotus/blue_py.png
    Textures/crystallotus/blue_pz.png
    Textures/crystallotus/sunset_nx.png
    Textures/crystallotus/sunset_ny.png
    Textures/crystallotus/sunset_nz.png
    Textures/crystallotus/sunset_px.png
    Textures/crystallotus/sunset_py.png
    Textures/crystallotus/sunset_pz.png
    Textures/heightmap.png
    Textures/noise.png
    Textures/quaternius/Bark_DeadTree_Normal.png
    Textures/quaternius/Bark_DeadTree.png
    Textures/quaternius/Bark_NormalTree_Normal.png
    Textures/quaternius/Bark_NormalTree.png
    Textures/quaternius/Leaf_Pine_C.png
    Textures/quaternius/Rocks_Desert_Diffuse.png
    Textures/quaternius/Rocks_Diffuse.png
    Textures/RealisticCharacter/role/Body.tga
    Textures/RealisticCharacter/role/Hair.tga
    Textures/RealisticCharacter/role/head.tga
    Textures/RealisticCharacter/weapon/Weapon.tga
    Textures/stainedglass.tga
    Textures/water.tga
    Textures/waterbump.png
)

add_executable(coursework ${SOURCES} ${HEADERS})
target_link_libraries(coursework nclgl)

# Copy resources to build directory
foreach(resource ${RESOURCES})
    configure_file(${CMAKE_SOURCE_DIR}/${resource} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${resource} COPYONLY)
endforeach()
