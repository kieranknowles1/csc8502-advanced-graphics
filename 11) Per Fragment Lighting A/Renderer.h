#pragma once

#include <memory>

#include "../nclgl/OGLRenderer.h"
#include "../nclgl/HeightMap.h"
#include "../nclgl/Camera.h"
#include "../nclgl/Light.h"
#include "../nclgl/Shader.h"
#include "../nclgl/ResourceManager.h"

class Renderer : public OGLRenderer
{
public:
	Renderer(Window &parent);
	~Renderer() override;

	void RenderScene() override;
	void UpdateScene(float dt) override;

protected:
	std::unique_ptr<ResourceManager> resourceManager;
	std::unique_ptr<HeightMap> heightMap;
	std::shared_ptr<Texture> heightMapTexture;
	std::unique_ptr<Shader> shader;

	std::unique_ptr<Camera> camera;
	std::unique_ptr<Light> light;
};
