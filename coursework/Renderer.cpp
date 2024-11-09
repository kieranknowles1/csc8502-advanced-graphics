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

    //skyTexture = resourceManager->getCubeMaps().get("crystallotus/sunset");
    oldSkybox = resourceManager->getCubeMaps().get("crystallotus/blue");
    newSkybox = resourceManager->getCubeMaps().get("crystallotus/sunset");

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
        0, 0, 0, (heightMapSize * 0.5f) + Vector3(0, 500, 0)
    );

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

    // The base class initialises RNG with a fixed seed, so we get consistent results

    sceneShader = resourceManager->getShaders().get({
        "ShadowSceneVert.glsl",
        "ShadowSceneFrag.frag"
    });

    // TODO: We need a version that works with tessellation
    // Should force the factor to 1 as shadows don't need that much detail
    shadowShader = resourceManager->getShaders().get({
        "ShadowVert.glsl",
        "ShadowFrag.glsl"
    });

    glGenTextures(1, &shadowTex);
    glBindTexture(GL_TEXTURE_2D, shadowTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, ShadowSize, ShadowSize, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

    glGenFramebuffers(1, &shadowFbo);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowFbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowTex, 0);
    glDrawBuffer(GL_NONE);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		throw std::runtime_error("Failed to create shadow framebuffer");
	}
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    shadowLightPos = camera->getPosition();
}

Renderer::~Renderer(void) {
    glDeleteFramebuffers(1, &oldFbo);
    glDeleteFramebuffers(1, &newFbo);
    glDeleteTextures(1, &oldTex);
    glDeleteTextures(1, &newTex);

    glDeleteTextures(1, &shadowTex);
    glDeleteFramebuffers(1, &shadowFbo);
}

void Renderer::UpdateScene(float dt) {
    time += dt;
    camera->update(dt);

    presentRoot->update(dt);
    futureRoot->update(dt);
}

void Renderer::RenderScene() {
    //viewMatrix = camera->buildViewMatrix();
    //projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);
    //// If we're fully in the past or future, the other scene is not visible
    //// Don't bother rendering it
    //if (timeWarp->getRatio() != 1)
    //{
    //    skyTexture = oldSkybox;
    //    drawTree(presentRoot.get(), oldFbo);
    //}
    //if (timeWarp->getRatio() != 0)
    //{
    //    skyTexture = newSkybox;
    //    drawTree(futureRoot.get(), newFbo);
    //}

    //combineBuffers();

    context.clear();
    buildNodeLists(presentRoot.get());
    // All software developers get headaches
    // 90% of them are self-inflicted by their own stupidity
    // For example, forgetting to draw shadows, then wondering why there are no shadows
    drawShadowScene();
    drawMainScene();

    auto glError = glGetError();
    if (glError != GL_NO_ERROR) {
		std::cerr << "OpenGL error: " << glError << std::endl;
	}
}

// TODO: Apply shadows directly to the FBO as part of deferred rendering
// This is just a POC for now
void Renderer::drawMainScene() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    BindShader(sceneShader.get());
    viewMatrix = camera->buildViewMatrix();
    projMatrix = Matrix4::Perspective(
        1.0f, 15000.0f, (float)width / (float)height, 45.0f
    );
    UpdateShaderMatrices();

    glUniform3f(sceneShader->getUniform("lightPos"),
        shadowLightPos.x, shadowLightPos.y, shadowLightPos.z
    );
    glUniform3f(sceneShader->getUniform("lightColor"), 1.0f, 1.0f, 1.0f);
    glUniform1f(sceneShader->getUniform("lightRadius"), 10000.0f);
    glUniform1f(sceneShader->getUniform("lightAttenuation"), 1.0f);

    glUniform1i(sceneShader->getUniform("diffuseTex"), 0);
    glUniform1i(sceneShader->getUniform("bumpTex"), 1);
    glUniform1i(sceneShader->getUniform("shadowTex"), 2);
    glUniform1i(sceneShader->getUniform("cubeTex"), 3);

    glUniform3f(sceneShader->getUniform("cameraPos"),
        camera->getPosition().x, camera->getPosition().y, camera->getPosition().z
    );

    glActiveTexture(GL_TEXTURE0);
    defaultMateriel.diffuse->bind();
    glActiveTexture(GL_TEXTURE1);
    defaultMateriel.normal->bind();
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, shadowTex);
    glActiveTexture(GL_TEXTURE3);
    oldSkybox->bind();

    drawNodes(context.opaqueNodes, /*shadowPass*/true); // Don't override materials for now
}

void Renderer::drawShadowScene() {
    glBindFramebuffer(GL_FRAMEBUFFER, shadowFbo);
    glClear(GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, ShadowSize, ShadowSize);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

    BindShader(shadowShader.get());

    viewMatrix = Matrix4::BuildViewMatrix(
		shadowLightPos,
		Vector3(0, 0, 0),
		Vector3(0, 1, 0)
	);
    projMatrix = Matrix4::Perspective(1, 10000, 1.0f, 45.0f);
    shadowMatrix = projMatrix * viewMatrix;
    UpdateShaderMatrices();

    drawNodes(context.opaqueNodes, /*shadowPass*/true);

    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, width, height);
}

void Renderer::combineBuffers() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    timeWarp->apply(*this);
}

std::vector<std::unique_ptr<SceneNode>> Renderer::loadTemplates(std::initializer_list<std::string> names, Vector3 scale, float yOff) {
	std::vector<std::unique_ptr<SceneNode>> templates;
	for (auto& name : names) {
		auto mesh = resourceManager->getMeshes().get(name + ".msh");
		auto tree = std::make_unique<SceneNode>(mesh);
		tree->setScale(scale);
		tree->setTransform(
			tree->getTransform() *
			Matrix4::Translation(Vector3(0, yOff, 0))
		);
		tree->setMateriels(Materiel::fromFile(resourceManager.get(), name + ".mat"));
		templates.push_back(std::move(tree));
	}
    return templates;
}

std::unique_ptr<SceneNode> Renderer::createPresentScene()
{
    auto root = std::make_unique<SceneNode>();

    auto heightMapNode = new SceneNode(
        this->heightMap
    );
    heightMapNode->setMateriel(heightMapMateriel);
    // TODO: Shadows don't currently support tessellation
    root->addChild(heightMapNode);

    auto presentOnly = new SceneNode();
    presentOnly->setTag("PresentOnly");
    root->addChild(presentOnly);

    auto treeParent = new SceneNode();
    treeParent->setTag("Trees");
    auto trees = loadTemplates({
        "quaternius/Pine_1",
        "quaternius/Pine_2",
        "quaternius/Pine_3",
        "quaternius/Pine_4",
        "quaternius/Pine_5"
    }, Vector3(10, 10, 10), -4);
    spawnTrees(treeParent, heightMap.get(), 500, trees);
    root->addChild(treeParent);

    auto rockParent = new SceneNode();
    auto rocks = loadTemplates({
		"quaternius/Rock_Medium_1",
        "quaternius/Rock_Medium_2",
        "quaternius/Rock_Medium_3",
	}, Vector3(10, 10, 10), -4);
    spawnTrees(rockParent, heightMap.get(), 1000, rocks);
    root->addChild(rockParent);


    Light* sun = new Light(1024); // Radius doesn't matter for sun lights
    sun->setTag("Sun");
    sun->setFacing(
        Vector3(1, 1, -1).Normalised()
    );
    sun->setType(Light::Type::Sun);
    root->addChild(sun);

    return root;
}

std::unique_ptr<SceneNode> Renderer::createFutureScene()
{
    // Clone the present scene, then use it as a base for the future scene
    auto node = presentRoot->deepCopy();

    // Deleting a node detaches it from its parent and deletes all children
    delete node->getTaggedChild("PresentOnly");

    // Replace trees with dead versions
    auto treeParent = node->getTaggedChild("Trees");
    // Technically, we only need the mesh and materiel, but creating a node doesn't have too much overhead
    auto deadTrees = loadTemplates({
		"quaternius/DeadTree_1",
		"quaternius/DeadTree_2",
        "quaternius/DeadTree_3",
        "quaternius/DeadTree_4",
        "quaternius/DeadTree_5"
	}, Vector3(7, 7, 7), -4);
    std::uniform_int_distribution<int> dist(0, deadTrees.size() - 1);
    for (auto child = treeParent->childrenBegin(); child != treeParent->childrenEnd(); child++) {
        auto& dead = deadTrees.at(dist(rng));
        (*child)->setMesh(dead->getMesh());
        (*child)->setMateriels(dead->getMateriels());
        (*child)->setScale(dead->getScale());
    }

	// TODO: Implement

    auto sun = dynamic_cast<Light*>(node->getTaggedChild("Sun"));
    sun->setColor(Vector4(
        255.0 / 255.0, 200.0 / 255.0, 150.0 / 255.0, 1.0
    ));
    sun->setFacing(Vector3(1, 1, 0).Normalised());

    return std::unique_ptr<SceneNode>(node);
}

void Renderer::spawnTrees(SceneNode* parent, Mesh* spawnOn, int count, const std::vector<std::unique_ptr<SceneNode>>& templates)
{
    std::uniform_int_distribution<int> pointDist(0, spawnOn->getVertexCount());
    std::uniform_int_distribution<int> templateDist(0, templates.size() - 1);
    std::uniform_real_distribution<float> angleDist(0, 360);
    std::uniform_real_distribution<float> scaleDist(0.8, 1.2);
    for (int i = 0; i < count; i++) {
        auto& choice = templates.at(templateDist(rng));
        auto instance = choice->deepCopy();
        // Pick a random point
        auto& point = spawnOn->getVertex(pointDist(rng));

        instance->setTransform(
            instance->getTransform() *
            Matrix4::Translation(point) *
            Matrix4::Rotation(angleDist(rng), Vector3(0, 1, 0)) // Don't rotate the translation, keep us on the same point
        );
        instance->setScale(instance->getScale() * scaleDist(rng));

        parent->addChild(instance);
    }
}
