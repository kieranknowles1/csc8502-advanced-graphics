#pragma once

#include "../nclgl/OGLRenderer.h"
#include "../nclgl/ResourceManager.h"
#include "../nclgl/SceneNode.h"
#include "../nclgl/Camera.h"
#include "../nclgl/HeightMap.h"

#include "CameraPath.h"
#include "TimeWarp.h"

// Renderer for the coursework
// Nothing here is thread safe
class Renderer : public OGLRenderer	{
public:
    Renderer(Window &parent, bool record);
    ~Renderer() override;
    void RenderScene() override;
    void UpdateScene(float msec) override;

    void setTimeWarpFactor(float factor) { timeWarp->setRatio(factor); }
    float getTimeWarpFactor() const { return timeWarp->getRatio(); }

    void summonLight();

    void toggleSun();

    void setWarpActive(bool active) {
        warpActive = active;
    }

    CameraPath* getCameraPath() { return cameraPath.get(); }

    // Write the current frame to a PNG file
    // Only works if `record` was set to true in the constructor
    void saveCurrentFrame(std::string filename);
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

    bool warpActive = false;
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
    
    // Subset of nodes that are visible to the shadow light
    // Kept between frames to reduce memory allocation
    void fillShadowVisible(SceneNode* from, Light* visibleFrom, std::vector<SceneNode*>& out) const;
    std::vector<SceneNode*> shadowVisible;

    void drawShadowScene(SceneNode* root, Light* light);

    void drawShadowLights(SceneNode* root) override;

    void combineBuffers();
    float time = 0;
    float waterLevel = 128;

    GLuint recordTexture;
    // The FBO that the final scene is rendered to
    // Either the screen or a render texture
    GLuint finalFbo;
    std::shared_ptr<Shader> passThroughShader;
    std::shared_ptr<Mesh> quad;
    bool recording = false;
};
