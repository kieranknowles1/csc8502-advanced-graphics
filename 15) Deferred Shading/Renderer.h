#pragma once

#include "../nclgl/OGLRenderer.h"
#include "../nclgl/Camera.h"
#include "../nclgl/HeightMap.h"
#include "../nclgl/Light.h"
#include "../nclgl/ResourceManager.h"
#include "../nclgl/Materiel.h"
#include "../nclgl/SceneNode.h"

class Renderer : public OGLRenderer {
public:
	constexpr static int LightCount = 512;

	Renderer(Window& parent);
	~Renderer() override;

	void RenderScene() override;
	void UpdateScene(float dt) override;
protected:
	void fillBuffers();
	void drawPointLights();
	void combineBuffers();

	// Allocate a screen-sized texture for use as a framebuffer
	GLuint generateScreenTexture(bool depth = false);

	std::unique_ptr<ResourceManager> resourceManager;

	std::shared_ptr<Shader> sceneShader;
	std::shared_ptr<Shader> pointlightShader;
	std::shared_ptr<Shader> combineShader;

	std::unique_ptr<Camera> camera;

	GLuint bufferFBO;
	GLuint bufferColorTex;
	GLuint bufferNormalTex;
	GLuint bufferDepthTex;

	GLuint pointLightFBO;
	GLuint lightDiffuseTex;
	GLuint lightSpecularTex;

	std::unique_ptr<HeightMap> heightMap;
	std::unique_ptr<Materiel> materiel;
	std::shared_ptr<Texture> earthTex;
	std::shared_ptr<Texture> earthBump;

	std::unique_ptr<SceneNode> root;

	std::vector<Light> pointLights;
	std::unique_ptr<Mesh> sphere;
	std::unique_ptr<Mesh> quad;
};
