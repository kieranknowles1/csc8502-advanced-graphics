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

    // Spawn `count` trees at random locations on `spawnOn` mesh as copies of `tree`
    void spawnTrees(SceneNode* parent, Mesh* spawnOn, int count, SceneNode* tree);

    std::unique_ptr<ResourceManager> resourceManager;

    std::unique_ptr<Camera> camera;

    std::shared_ptr<HeightMap> heightMap;
    std::shared_ptr<Mesh> cube;

    std::unique_ptr<SceneNode> presentRoot;
    std::unique_ptr<SceneNode> futureRoot;
};
