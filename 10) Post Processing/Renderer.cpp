#include "Renderer.h"

void setUnfilteredClamp() {
	// Make sure we don't sample outside the texture, which could cause distortion
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	// Don't filter, as this would reduce quality
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}

Renderer::Renderer(Window& parent)
	: OGLRenderer(parent)
	, camera(new Camera(-25, 225, 0, Vector3(-150, 250, -150)))
	, heightMap(new HeightMap(TEXTUREDIR "noise.png"))
	, quad(Mesh::GenerateQuad())
	, postProcess(new PostProcess::Blur(10))
{
	resourceManager = std::make_unique<ResourceManager>();
	heightMapTexture = resourceManager->getTexture(
		"Barren Reds.JPG", SOIL_FLAG_MIPMAPS
	);

	projMatrix = Matrix4::Perspective(1.0f, 10000.0f,
		(float)width / (float)height, 45.0f);

	sceneShader = new Shader("TexturedVertex.glsl", "TexturedFragment.glsl");

	if (!sceneShader->LoadSuccess())
		return;

	setTextureRepeating(heightMapTexture->getId(), true);

	// Generate our scene's depth texture
	glGenTextures(1, &bufferDepthTex);
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);
	setUnfilteredClamp();
	// Create the texture big enough for the screen containing
	// a packed 24-bit depth and 8-bit stencil value
	glTexImage2D(
		GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height,
		0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr
	);

	// Generate our scene's colour textures
	glGenTextures(2, postProcessScratch);
	for (int i = 0; i < 2; i++) {
		glBindTexture(GL_TEXTURE_2D, postProcessScratch[i]);
		setUnfilteredClamp();
		glTexImage2D(
			GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0,
			GL_RGBA, GL_UNSIGNED_BYTE, nullptr
		);
	}
	glGenTextures(1, &sceneBuffer);
	glBindTexture(GL_TEXTURE_2D, sceneBuffer);
	setUnfilteredClamp();
	glTexImage2D(
		GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, nullptr
	);

	// Generate frame buffer objects and attach textures to them
	glGenFramebuffers(1, &bufferFBO);
	glGenFramebuffers(1, &processFBO);

	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glFramebufferTexture2D(
		GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, bufferDepthTex, 0);
	glFramebufferTexture2D(
		GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, bufferDepthTex, 0);
	glFramebufferTexture2D(
		GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, sceneBuffer, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		return;
	if (!bufferDepthTex || !postProcessScratch[0])
		return;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_DEPTH_TEST);

	init = true;
}

Renderer::~Renderer()
{
	delete sceneShader;
	delete camera;
	delete heightMap;
	delete quad;

	glDeleteTextures(2, postProcessScratch);
	glDeleteTextures(1, &bufferDepthTex);
	glDeleteTextures(1, &sceneBuffer);
	glDeleteFramebuffers(1, &bufferFBO);
	glDeleteFramebuffers(1, &processFBO);
}

void Renderer::RenderScene()
{
	drawScene();
	auto postResult = postProcess->apply(
		*this, sceneBuffer, processFBO, postProcessScratch
	);
	presentScene(postResult);
}

void Renderer::UpdateScene(float dt)
{
	camera->update(dt);
	viewMatrix = camera->buildViewMatrix();
}

void Renderer::presentScene(GLuint texture)
{
	// Render the final image to the screen
	// This time we do need to clear, as we are rendering the whole screen

	// This function didn't work
	// But then it did
	// Yet one question remains
	// What the fuck did I do?
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	BindShader(sceneShader);
	modelMatrix.ToIdentity();
	projMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	UpdateShaderMatrices();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(sceneShader->getUniform("diffuseTex"), 0);
	quad->Draw();
}

void Renderer::drawScene()
{
	// Render the scene to our FBO
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	BindShader(sceneShader);
	projMatrix = Matrix4::Perspective(1.0f, 10000.0f,
		(float)width / (float)height, 45.0f);
	UpdateShaderMatrices();

	glActiveTexture(GL_TEXTURE0);
	glUniform1i(sceneShader->getUniform("diffuseTex"), 0);
	heightMapTexture->bind();

	heightMap->Draw();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
