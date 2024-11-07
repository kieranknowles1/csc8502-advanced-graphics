#include "Renderer.h"

#include "../nclgl/HeightMap.h"
#include "../nclgl/CubeBot.h"
#include "../nclgl/Light.h"

Renderer::Renderer(Window& parent)
    : OGLRenderer(parent)
    , cube(resourceManager->getMeshes().get({"OffsetCubeY.msh"}))
{
    setDefaultMateriel({
        resourceManager->getTextures().get({"Barren Reds.JPG", SOIL_FLAG_MIPMAPS, true}),
        resourceManager->getTextures().get({"Barren RedsDOT3.JPG", SOIL_FLAG_MIPMAPS, true}),
        resourceManager->getShaders().get({"BumpVertex.glsl", "BufferFragment.glsl"})
    });

    setPointLightShader(resourceManager->getShaders().get({"PointLightVertex.glsl", "PointLightFrag.glsl"}));
    setCombineShader(resourceManager->getShaders().get({"CombineVert.glsl", "CombineFrag.glsl"}));

    skyTexture = resourceManager->getCubeMaps().get("rusted"); // TODO: Use a different texture for the future scene
    skyShader = resourceManager->getShaders().get({"SkyboxVertex.glsl", "SkyboxFragment.glsl"});

    heightMap = std::make_shared<HeightMap>(TEXTUREDIR "heightmap.png", Vector3(8, 2, 8));
    auto tesselateShader = resourceManager->getShaders().get({
        "TessVertex.glsl",
        "BufferFragment.glsl",
        "",
        "TessControl.glsl",
        "TessEval.glsl"
    });
    heightMapMateriel = Materiel({
        defaultMateriel.diffuse,
        defaultMateriel.normal,
        tesselateShader,
        resourceManager->getTextures().get({"noise.png", SOIL_FLAG_MIPMAPS, true})
    });

    auto heightMapSize = heightMap->getSize();
    camera = std::make_unique<Camera>(
        parent.GetKeyboard(), parent.GetMouse(),
        -45, 0, 0, (heightMapSize * 0.5f) + Vector3(0, 500, 0)
    );

    projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);

    presentRoot = createPresentScene();
    futureRoot = createFutureScene();

    glGenFramebuffers(1, &oldFbo);
    oldTex = generateScreenTexture(false, GL_MIRRORED_REPEAT);
    glBindFramebuffer(GL_FRAMEBUFFER, oldFbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, oldTex, 0);

    glGenFramebuffers(1, &newFbo);
    newTex = generateScreenTexture(false, GL_MIRRORED_REPEAT);
    glBindFramebuffer(GL_FRAMEBUFFER, newFbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, newTex, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    timeWarp = std::make_unique<TimeWarp>(resourceManager.get(), oldTex, newTex);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glCullFace(GL_BACK);
}

Renderer::~Renderer(void) {
    glDeleteFramebuffers(1, &oldFbo);
    glDeleteFramebuffers(1, &newFbo);
    glDeleteTextures(1, &oldTex);
    glDeleteTextures(1, &newTex);
}

void Renderer::UpdateScene(float dt) {
    time += dt;
    camera->update(dt);
    viewMatrix = camera->buildViewMatrix();

    presentRoot->update(dt);
    futureRoot->update(dt);
}

void Renderer::RenderScene()	{
    //glClearColor(0.2f,0.2f,0.2f,1.0f);
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //BindShader(getDefaultMateriel().shader.get());
    //UpdateShaderMatrices();

    // If we're fully in the past or future, the other scene is not visible
    // Don't bother rendering it
    if (timeWarp->getRatio() != 1)
        drawTree(presentRoot.get(), oldFbo);
    if (timeWarp->getRatio() != 0)
        drawTree(futureRoot.get(), newFbo);

    combineBuffers();

    //drawTree(presentRoot.get());
}

void Renderer::combineBuffers() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    timeWarp->apply(*this);
}

std::unique_ptr<SceneNode> Renderer::createPresentScene()
{
    auto root = std::make_unique<SceneNode>();

    auto heightMapNode = new SceneNode(
        this->heightMap
    );
    heightMapNode->setMateriel(heightMapMateriel);
    root->addChild(heightMapNode);

    auto treeParent = new SceneNode();

    auto templates = std::vector<SceneNode*>();
    auto loadTree = [&](const std::string& name, Vector3 scale, float yOff) {
        auto mesh = resourceManager->getMeshes().get(name + ".msh");
        auto tree = new SceneNode(mesh);
        tree->setScale(scale);
        tree->setTransform(
            tree->getTransform() *
            Matrix4::Translation(Vector3(0, yOff, 0))
        );
        tree->setMateriels(Materiel::fromFile(resourceManager.get(), name + ".mat"));
        templates.push_back(tree);
        };
    loadTree("quaternius/Pine_1", Vector3(10, 10, 10), -40);
    loadTree("quaternius/Pine_2", Vector3(10, 10, 10), -40);
    loadTree("quaternius/Pine_3", Vector3(10, 10, 10), -40);
    loadTree("quaternius/Pine_4", Vector3(10, 10, 10), -40);
    loadTree("quaternius/Pine_5", Vector3(10, 10, 10), -40);

    spawnTrees(treeParent, heightMap.get(), 500, templates);
    for (auto& tree : templates) {
        delete tree;
    }

    root->addChild(treeParent);

    Light* sun = new Light(1024); // Radius doesn't matter for sun lights
    sun->setFacing(
        Vector3(-1, 1, 0).Normalised()
    );
    sun->setType(Light::Type::Sun);
    root->addChild(sun);

    return root;
}

std::unique_ptr<SceneNode> Renderer::createFutureScene()
{
	// TODO: Implement
    auto node = std::make_unique<SceneNode>(heightMap);
    node->setColor(Vector4(0.5f, 0.5f, 1.0f, 1.0f));
    node->setMateriel(heightMapMateriel);

    Light* sun = new Light(1024); // Radius doesn't matter for sun lights
    sun->setFacing(
        Vector3(-1, 1, 1).Normalised()
    );
    sun->setType(Light::Type::Sun);
    node->addChild(sun);

    return node;
}

void Renderer::spawnTrees(SceneNode* parent, Mesh* spawnOn, int count, const std::vector<SceneNode*>& templates)
{
    std::uniform_int_distribution<int> pointDist(0, spawnOn->getVertexCount());
    std::uniform_int_distribution<int> templateDist(0, templates.size() - 1);
    for (int i = 0; i < count; i++) {
        auto choice = templates.at(templateDist(rng));
        auto instance = choice->deepCopy();
        // Pick a random point
        auto& point = spawnOn->getVertex(pointDist(rng));

        instance->setTransform(
            instance->getTransform() *
            Matrix4::Translation(point)
        );

        parent->addChild(instance);
    }
}
