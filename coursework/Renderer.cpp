#include "Renderer.h"

#include "../nclgl/HeightMap.h"
#include "../nclgl/SkeletonAnim.h"
#include "../nclgl/Light.h"

Renderer::Renderer(Window& parent)
    : OGLRenderer(parent)
    , cube(resourceManager->getMeshes().get({"OffsetCubeY.msh"}))
{
    setDefaultMateriel({
        resourceManager->getTextures().get({"Barren Reds.JPG", SOIL_FLAG_MIPMAPS, true}),
        resourceManager->getTextures().get({"Barren RedsDOT3.JPG", SOIL_FLAG_MIPMAPS, true}),
        resourceManager->getShaders().get({"BumpVertex.glsl", "BufferFragment.glsl"}),
        resourceManager->getShaders().get({"ShadowVert.glsl", "ShadowFrag.glsl"})
    });

    setPointLightShader(resourceManager->getShaders().get({"DeferredLight.vert", "DeferredLight.frag"}));
    setCombineShader(resourceManager->getShaders().get({"CombineVert.glsl", "CombineFrag.glsl"}));

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
    auto tesselateShadowShader = resourceManager->getShaders().get({
        // TessVertex has several unused attributes when used for shadows,
        // assume the compiler will optimise them out making them the same
        // speed as a specialised shadow shader
        "TessVertex.glsl",
        "ShadowFrag.glsl",
        "",
        "TessControlShadow.glsl",
        "TessEvalShadow.glsl"
    });
    heightMapMateriel = Materiel({
        defaultMateriel.diffuse,
        defaultMateriel.normal,
        tesselateShader,
        tesselateShadowShader,
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
    if (!lockFrustum) {
        viewFrustum.fillFromMatrix(projMatrix * camera->buildViewMatrix());
    }

    presentRoot->update(dt);
    futureRoot->update(dt);
}

void Renderer::RenderScene() {
    viewMatrix = camera->buildViewMatrix();
    projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);
    // If we're fully in the past or future, the other scene is not visible
    // Don't bother rendering it
    if (timeWarp->getRatio() != 1)
    {
       skyTexture = oldSkybox;
       drawTree(presentRoot.get(), oldFbo);
    }
    if (timeWarp->getRatio() != 0)
    {
       skyTexture = newSkybox;
       drawTree(futureRoot.get(), newFbo);
    }

    combineBuffers();
}

void Renderer::drawShadowLights(SceneNode* root) {
    beginLightPass();
    glUniform1i(pointLightShader->getUniform("useShadows"), 1);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, shadowTex);
    glUniform1i(pointLightShader->getUniform("shadowTex"), 2);

    for (auto& shadowCaster : context.shadowLights) {
        drawShadowScene(root, shadowCaster);
        glBindFramebuffer(GL_FRAMEBUFFER, deferredLightFbo);
        BindShader(pointLightShader.get());
        shadowCaster->bind(*this);
        sphere->Draw();
    }

    glUniform1i(pointLightShader->getUniform("useShadows"), 0);
    endLightPass();
}

//#define LAZY_SHADOWS

void Renderer::fillShadowVisible(SceneNode* from, Light* visibleFrom, std::vector<SceneNode*>& out) const {
    Frustum lightFrustum(
        visibleFrom->getShadowProjMatrix() * visibleFrom->getShadowViewMatrix()
    );
    // Using only camera visible nodes isn't strictly correct,
    // and clips some objects that should cast shadows
    // but is good enought and less CPU intensive (according to a very unscientific test)
    // The test was 50% science, in that I can do it again but I didn't write anything down
#ifdef LAZY_SHADOWS
    for (auto& cameraVisible : context.opaqueNodes) {
        if (lightFrustum.inFrustum(*cameraVisible)) {
			out.push_back(cameraVisible);
		}
    }
#else
    if (lightFrustum.inFrustum(*from)) {
		out.push_back(from);
	}
    for (auto child = from->childrenBegin(); child != from->childrenEnd(); child++) {
		fillShadowVisible(*child, visibleFrom, out);
	}
#endif // LAZY_SHADOWS
}

void Renderer::drawShadowScene(SceneNode* root, Light* light) {
    auto oldView = viewMatrix;
    auto oldProj = projMatrix;
    glBindFramebuffer(GL_FRAMEBUFFER, shadowFbo);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glViewport(0, 0, ShadowSize, ShadowSize);
    // Need to disable some light-specific settings
    // Cull front faces, so that the depth buffer is filled with back faces
    // The back face won't show light due to its incident angle
    glCullFace(GL_FRONT);
    // Write to the depth buffer only
    glDepthMask(GL_TRUE);
    // This has to be after enabling depth writing on Windows, but not on Linux
    // Seems like a driver bug
    glClear(GL_DEPTH_BUFFER_BIT);
    glDepthFunc(GL_LEQUAL);

    viewMatrix = light->getShadowViewMatrix();
    projMatrix = light->getShadowProjMatrix();
    shadowMatrix = projMatrix * viewMatrix;
    UpdateShaderMatrices();

    // As with filtering nodes for the main camera, we want to reuse memory
    // when possible, clear() doesn't free anything without shrink_to_fit
    shadowVisible.clear();
    fillShadowVisible(root, light, shadowVisible);
    drawNodes(shadowVisible, /*shadowPass*/true);

    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glEnable(GL_CULL_FACE);
    glDepthFunc(GL_ALWAYS);
    glDepthMask(GL_FALSE);
    glViewport(0, 0, width, height);
    viewMatrix = oldView;
    projMatrix = oldProj;
}

void Renderer::summonLight() {
    std::cout << "Spawning light\n";
    auto& parent = timeWarp->getRatio() == 0 ? presentRoot : futureRoot;

    Light* light = new Light(2000, 0.01);
    std::uniform_real_distribution<float> colorDist(0, 1);
    light->setColor(Vector4(colorDist(rng), colorDist(rng), colorDist(rng), 1));
    light->setCastShadows(true);
    light->setShadowProjMatrix(
        Matrix4::Perspective(10, 2000, 1, 45)
    );
    light->setShadowViewMatrix(camera->buildViewMatrix());

    light->setTransform(
        Matrix4::Translation(camera->getPosition())
    );
    
    parent->addChild(light);
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
        tree->setBoundingRadius(mesh->getBoundingRadius());
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
    heightMapNode->setBoundingRadius(3 * heightMap->getSize().Length());
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
    sun->setColor(
        Vector4(0.5, 0.5, 0.5, 1)
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

    auto animatedNode = new SkeletonAnim(
        resourceManager.get(),
        "Role_T.msh",
        "Role_T.anm",
        "Role_T.mat"
    );
    animatedNode->setScale(Vector3(20, 20, 20));
    std::vector<std::unique_ptr<SceneNode>> templates;
    templates.push_back(std::unique_ptr<SceneNode>(animatedNode));
    
    auto animParent = new SceneNode();
    spawnTrees(animParent, heightMap.get(), 50, templates);
    node->addChild(animParent);

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
