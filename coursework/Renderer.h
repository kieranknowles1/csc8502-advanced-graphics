#pragma once

#include "../nclgl/OGLRenderer.h"
#include "../nclgl/ResourceManager.h"
#include "../nclgl/SceneNode.h"
#include "../nclgl/Camera.h"
#include "../nclgl/HeightMap.h"

class Renderer : public OGLRenderer	{
public:
    Renderer(Window &parent);
    ~Renderer() override;
    void RenderScene() override;
    void UpdateScene(float msec) override;
protected:
    std::unique_ptr<SceneNode> createPresentScene();
    std::unique_ptr<SceneNode> createFutureScene();

    void spawnTrees(SceneNode* parent, Mesh* mesh, int count, SceneNode* tree);

    std::unique_ptr<ResourceManager> resourceManager;

    std::unique_ptr<Camera> camera;

    std::unique_ptr<HeightMap> heightMap;
    // TODO: Handle this in the resource manager
    std::unique_ptr<Mesh> cube;

    std::unique_ptr<SceneNode> presentRoot;
    std::unique_ptr<SceneNode> futureRoot;
};
