#pragma once

#include "../nclgl/OGLRenderer.h"
#include "../nclgl/ResourceManager.h"

class Renderer : public OGLRenderer	{
public:
    Renderer(Window &parent);
    ~Renderer() override;
    void RenderScene() override;
    void UpdateScene(float msec) override;
protected:
    std::unique_ptr<ResourceManager> resourceManager;

    Mesh* triangle;
    Shader* basicShader;
};
