#pragma once

#include "../nclgl/OGLRenderer.h"
#include "../nclgl/ResourceManager.h"
#include "../nclgl/SceneNode.h"
#include "../nclgl/Camera.h"
#include "../nclgl/HeightMap.h"

#include "CameraPath.h"
#include "TimeWarp.h"

class Renderer : public OGLRenderer	{
public:
    Renderer(Window &parent);
    ~Renderer() override;
    void RenderScene() override;
    void UpdateScene(float msec) override;

    void setTimeWarpFactor(float factor) { timeWarp->setRatio(factor); }
    float getTimeWarpFactor() const { return timeWarp->getRatio(); }

    void summonLight();

    void toggleSun();

    CameraPath* getCameraPath() { return cameraPath.get(); }
protected:
    std::unique_ptr<SceneNode> createPresentScene();
    std::unique_ptr<SceneNode> createFutureScene();
    std::unique_ptr<CameraPath> createCameraPath();

    std::vector<std::unique_ptr<SceneNode>> loadTemplates(std::initializer_list<std::string> names, Vector3 scale, float yOff);

    // Spawn `count` trees at random locations on `spawnOn` mesh as copies of `tree`
    void spawnTrees(SceneNode* parent, Mesh* spawnOn, int count, const std::vector<std::unique_ptr<SceneNode>>& templates);

    std::shared_ptr<HeightMap> heightMap;
    Materiel heightMapMateriel;

    std::unique_ptr<SceneNode> presentRoot;
    std::unique_ptr<SceneNode> futureRoot;

    std::unique_ptr<TimeWarp> timeWarp;
    std::unique_ptr<CameraPath> cameraPath;

    GLuint oldFbo;
    GLuint oldTex;
    std::shared_ptr<Texture> oldSkybox;

    GLuint newFbo;
    GLuint newTex;
    std::shared_ptr<Texture> newSkybox;

    std::shared_ptr<Shader> combineShader;

    const static int ShadowSize = 2048;
    GLuint shadowTex;
    GLuint shadowFbo;
    
    Light* createLight(Vector3 position, float pitch, float yaw);
    Vector4 generateColor();
    
    // Subset of nodes that are visible to both the camera and the shadow light
    void fillShadowVisible(SceneNode* from, Light* visibleFrom, std::vector<SceneNode*>& out) const;
    std::vector<SceneNode*> shadowVisible;

    void drawShadowScene(SceneNode* root, Light* light);

    void drawShadowLights(SceneNode* root) override;

    void combineBuffers();
    float time = 0;
};
