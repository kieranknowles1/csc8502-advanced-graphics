#include "Renderer.h"

#include "../nclgl/HeightMap.h"
#include "../nclgl/CubeBot.h"

Renderer::Renderer(Window& parent)
    : OGLRenderer(parent)
    , resourceManager(std::make_unique<ResourceManager>())
    , cube(resourceManager->getMeshes().get({"OffsetCubeY.msh"}))
{
    setDefaultMateriel({
        resourceManager->getTextures().get({"Barren Reds.JPG", SOIL_FLAG_MIPMAPS, true}),
        resourceManager->getTextures().get({"Barren RedsDOT3.JPG", SOIL_FLAG_MIPMAPS, true}),
        resourceManager->getShaders().get({"BumpVertex.glsl", "BufferFragment.glsl"})
    });

    heightMap = std::make_shared<HeightMap>(TEXTUREDIR "heightmap.png", 4, Vector3(8, 2, 8));

    auto heightMapSize = heightMap->getSize();
    camera = std::make_unique<Camera>(
        parent.GetKeyboard(), parent.GetMouse(),
        -45, 0, 0, (heightMapSize * 0.5f) + Vector3(0, 500, 0)
    );

    projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);

    presentRoot = createPresentScene();
    futureRoot = createFutureScene();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
}

Renderer::~Renderer(void)	{
    // Free resources so that ResourceManager doesn't see them as leaked
    setDefaultMateriel({
        nullptr,
        nullptr,
        nullptr
    });
}

void Renderer::UpdateScene(float dt) {
    camera->update(dt);
    viewMatrix = camera->buildViewMatrix();

    presentRoot->update(dt);
    futureRoot->update(dt);
}

void Renderer::RenderScene()	{
    glClearColor(0.2f,0.2f,0.2f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    BindShader(getDefaultMateriel().shader.get());
    UpdateShaderMatrices();

    drawTree(presentRoot.get());
}

std::unique_ptr<SceneNode> Renderer::createPresentScene()
{
    auto root = std::make_unique<SceneNode>();

    root->addChild(new SceneNode(
        heightMap
    ));

    auto treeParent = new SceneNode();
    // TODO: Use a proper mesh
    auto tree = std::make_unique<CubeBot>(cube);

    spawnTrees(treeParent, heightMap.get(), 500, tree.get());
    root->addChild(treeParent);

    return root;
}

std::unique_ptr<SceneNode> Renderer::createFutureScene()
{
	// TODO: Implement
    return std::make_unique<SceneNode>();
}

void Renderer::spawnTrees(SceneNode* parent, Mesh* spawnOn, int count, SceneNode* tree)
{
    std::uniform_int_distribution<int> pointDist(0, spawnOn->getVertexCount());
    for (int i = 0; i < count; i++) {
        auto instance = tree->deepCopy();
        // Pick a random point
        auto point = spawnOn->getVertex(pointDist(rng));

        instance->setTransform(
            Matrix4::Translation(point)
        );

        parent->addChild(instance);
    }
}
