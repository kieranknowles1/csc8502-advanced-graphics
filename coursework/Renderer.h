#pragma once

#include "../nclgl/OGLRenderer.h"
#include "../nclgl/ResourceManager.h"
#include "../nclgl/SceneNode.h"
#include "../nclgl/Camera.h"
#include "../nclgl/HeightMap.h"

#include "TimeWarp.h"

class Renderer : public OGLRenderer	{
public:
    Renderer(Window &parent);
    ~Renderer() override;
    void RenderScene() override;
    void UpdateScene(float msec) override;

    void setTimeWarpFactor(float factor) { timeWarp->setRatio(factor); }
    float getTimeWarpFactor() const { return timeWarp->getRatio(); }
protected:
    std::unique_ptr<SceneNode> createPresentScene();
    std::unique_ptr<SceneNode> createFutureScene();

    std::vector<std::unique_ptr<SceneNode>> loadTemplates(std::initializer_list<std::string> names, Vector3 scale, float yOff);

    // Spawn `count` trees at random locations on `spawnOn` mesh as copies of `tree`
    void spawnTrees(SceneNode* parent, Mesh* spawnOn, int count, const std::vector<std::unique_ptr<SceneNode>>& templates);

    std::shared_ptr<HeightMap> heightMap;
    Materiel heightMapMateriel;
    std::shared_ptr<Mesh> cube;

    std::unique_ptr<SceneNode> presentRoot;
    std::unique_ptr<SceneNode> futureRoot;

    std::unique_ptr<TimeWarp> timeWarp;

    GLuint oldFbo;
    GLuint oldTex;
    std::shared_ptr<Texture> oldSkybox;

    GLuint newFbo;
    GLuint newTex;
    std::shared_ptr<Texture> newSkybox;

    std::shared_ptr<Shader> combineShader;

    // TODO: Support multiple shadow casters
    const static int ShadowSize = 2048;
    GLuint shadowTex;
    GLuint shadowFbo;
    std::shared_ptr<Shader> shadowShader;
    Vector3 shadowLightPos;

    void drawShadowScene(Light* light);

    void drawShadowLights() override;

    void combineBuffers();
    float time = 0;
};
