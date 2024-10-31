#pragma once

#include "../nclgl/OGLRenderer.h"
#include "../nclgl/Camera.h"
#include "../nclgl/HeightMap.h"
#include "../nclgl/ResourceManager.h"
#include "../nclgl/PostProcess/Blur.h"


class Renderer : public OGLRenderer
{
public:
	Renderer(Window &parent);
	~Renderer() override;

	void RenderScene() override;
	void UpdateScene(float dt) override;
protected:
	void presentScene(GLuint texture);
	void drawScene();

	std::unique_ptr<ResourceManager> resourceManager;

	PostProcess::Stage* postProcess;

	Shader* sceneShader;

	Camera* camera;

	// For drawing the final FBO to the screen
	Mesh* quad;
	HeightMap* heightMap;
	std::shared_ptr<Texture> heightMapTexture;
	// Render the scene to this FBO
	GLuint bufferFBO;
	GLuint sceneBuffer;
	// Render post-processing to this FBO
	GLuint processFBO;
	// We need two colour buffers, one for each FBO
	GLuint postProcessScratch[2];
	// Only the initial pass needs a depth buffer
	GLuint bufferDepthTex;
};

