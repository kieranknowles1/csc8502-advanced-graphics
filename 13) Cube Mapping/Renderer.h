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
	void drawSkybox();
	void drawHeightmap();
	void drawWater();

	std::unique_ptr<ResourceManager> resourceManager;

	std::unique_ptr<Shader> lightShader;
	std::unique_ptr<Shader> skyboxShader;

	std::unique_ptr<HeightMap> heightMap;
	std::unique_ptr<Mesh> quad;


	std::shared_ptr<ManagedTexture> cubeMap;
	std::shared_ptr<ManagedTexture> waterTex;
	std::shared_ptr<ManagedTexture> waterBump;
	std::shared_ptr<ManagedTexture> earthTex;
	std::shared_ptr<ManagedTexture> earthBump;

	std::unique_ptr<Camera> camera;
	std::unique_ptr<Light> light;

	float waterRotate;
	float waterCycle;
};
