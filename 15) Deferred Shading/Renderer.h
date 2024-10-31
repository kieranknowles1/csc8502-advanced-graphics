#pragma once

#include "../nclgl/OGLRenderer.h"
#include "../nclgl/Camera.h"
#include "../nclgl/HeightMap.h"
#include "../nclgl/Light.h"
#include "../nclgl/ResourceManager.h"

class Renderer : public OGLRenderer {
public:
	constexpr static int LightCount = 32;

	Renderer(Window& parent);
	~Renderer() override;

	void RenderScene() override;
	void UpdateScene(float dt) override;
protected:
	void fillBuffers();
	void drawPointLights();
	void combineBuffers();

	void generateScreenTexture(bool depth);

	std::unique_ptr<ResourceManager> resourceManager;

	std::unique_ptr<Shader> sceneShader;
	std::unique_ptr<Shader> pointlightShader;
	std::unique_ptr<Shader> combineShader;

	std::unique_ptr<Camera> camera;

	GLuint bufferFBO;
	GLuint bufferColorTex;
	GLuint bufferNormalTex;
	GLuint bufferDepthTex;

	GLuint pointLightFBO;
	GLuint lightDiffuseTex;
	GLuint lightSpecularTex;

	std::unique_ptr<HeightMap> heightMap;
	std::shared_ptr<Texture> earthTex;
	std::shared_ptr<Texture> earthBump;

	std::vector<Light> pointLights;
	std::unique_ptr<Mesh> sphere;
	std::unique_ptr<Mesh> quad;
};
