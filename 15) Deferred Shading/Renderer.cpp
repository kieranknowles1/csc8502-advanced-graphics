#include "Renderer.h"

#include <random>

Renderer::Renderer(Window& parent)
	: OGLRenderer(parent)
	, resourceManager(std::make_unique<ResourceManager>())

	, sphere(Mesh::LoadFromMeshFile("Sphere.msh"))
	, quad(Mesh::GenerateQuad())
	, heightMap(std::make_unique<HeightMap>(TEXTUREDIR"noise.png"))

	, earthTex(resourceManager->getTextures().get({"Barren Reds.JPG", SOIL_FLAG_MIPMAPS, true}))
	, earthBump(resourceManager->getTextures().get({"Barren RedsDOT3.JPG", SOIL_FLAG_MIPMAPS, true}))

	, sceneShader(resourceManager->getShaders().get({"BumpVertex.glsl", "BufferFragment.glsl"}))
	, pointlightShader(resourceManager->getShaders().get({"PointLightVertex.glsl", "PointLightFrag.glsl"}))
	, combineShader(resourceManager->getShaders().get({"CombineVert.glsl", "CombineFrag.glsl"}))
{
	if (!sceneShader->LoadSuccess() || !pointlightShader->LoadSuccess() || !combineShader->LoadSuccess()) {
		throw std::runtime_error("Failed to load shaders");
	}

	auto heightMapSize = heightMap->getSize();
	camera = std::make_unique<Camera>(parent.GetKeyboard(), parent.GetMouse(), -45, 0, 0, (heightMapSize * 0.5f) + Vector3(0, 500, 0));

	std::mt19937 rng(0); // Keep things consistent
	std::uniform_real_distribution<float> xDist(0, heightMapSize.x);
	std::uniform_real_distribution<float> zDist(0, heightMapSize.z);
	std::uniform_real_distribution<float> yDist(0, 150);
	std::uniform_real_distribution<float> colorDist(0, 1);
	std::uniform_real_distribution<float> radiusDist(250, 500);
	for (int i = 0; i < LightCount; i++) {
		Vector3 position = Vector3(xDist(rng), yDist(rng), zDist(rng));
		Vector3 color = Vector3(colorDist(rng), colorDist(rng), colorDist(rng));
		float radius = radiusDist(rng);

		pointLights.emplace_back(position, color, radius);
	}

	glGenFramebuffers(1, &bufferFBO); // For the first pass - draw scene into G-Buffer
	// TODO: Should lighting be done using PostProcess::Stages?
	glGenFramebuffers(1, &pointLightFBO); // Second pass - draw point lights into light buffer
	// Final pass is direct to the screen, but could also be post-processed

	bufferDepthTex = generateScreenTexture(/*depth=*/true);
	bufferColorTex = generateScreenTexture();
	bufferNormalTex = generateScreenTexture();

	lightDiffuseTex = generateScreenTexture();
	lightSpecularTex = generateScreenTexture();

	// We need to instruct OpenGL on which color attachments we'll be drawing into
	GLenum buffers[] = {
		GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1
	};

	// Attach textures to the framebuffers
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColorTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, bufferNormalTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, bufferDepthTex, 0);
	glDrawBuffers(2, buffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		throw std::runtime_error("Framebuffer not complete");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, pointLightFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, lightDiffuseTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, lightSpecularTex, 0);
	glDrawBuffers(2, buffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		throw std::runtime_error("Framebuffer not complete");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
}

GLuint Renderer::generateScreenTexture(bool depth) {
	GLuint tex;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	auto format = depth ? GL_DEPTH_COMPONENT24 : GL_RGBA8;
	auto type = depth ? GL_DEPTH_COMPONENT : GL_RGBA;

	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, type, GL_UNSIGNED_BYTE, nullptr);
	glBindTexture(GL_TEXTURE_2D, 0);

	return tex;
}

Renderer::~Renderer()
{
	glDeleteTextures(1, &bufferColorTex);
	glDeleteTextures(1, &bufferNormalTex);
	glDeleteTextures(1, &bufferDepthTex);

	glDeleteTextures(1, &lightDiffuseTex);
	glDeleteTextures(1, &lightSpecularTex);

	glDeleteFramebuffers(1, &bufferFBO);
	glDeleteFramebuffers(1, &pointLightFBO);

}

void Renderer::RenderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	fillBuffers();
	drawPointLights();
	combineBuffers();
}

// Fill the G-Buffer with the scene
void Renderer::fillBuffers() {
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	BindShader(sceneShader.get());
	glUniform1i(sceneShader->getUniform("diffuseTex"), 0);
	glUniform1i(sceneShader->getUniform("bumpTex"), 1);

	glActiveTexture(GL_TEXTURE0);
	earthTex->bind();

	// We don't do lighting yet, so just store the normals for later
	glActiveTexture(GL_TEXTURE1);
	earthBump->bind();

	modelMatrix.ToIdentity();
	viewMatrix = camera->buildViewMatrix();
	projMatrix = Matrix4::Perspective(1, 10000, (float)width / (float)height, 45);

	UpdateShaderMatrices();
	heightMap->Draw();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::drawPointLights() {
	glBindFramebuffer(GL_FRAMEBUFFER, pointLightFBO);
	BindShader(pointlightShader.get());

	// This pass will be blended with the scene, so should be cleared to black
	// otherwise the scene will be washed out
	glClearColor(0, 0, 0, 1);
	// We have no depth buffer
	glClear(GL_COLOR_BUFFER_BIT);

	// Combine lights additively
	glBlendFunc(GL_ONE, GL_ONE);
	// Draw each sphere exactly once, even if the camera is inside it
	glCullFace(GL_FRONT);
	// Don't occlude anything
	glDepthFunc(GL_ALWAYS);
	glDepthMask(GL_FALSE);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);
	glUniform1i(pointlightShader->getUniform("depthTex"), 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, bufferNormalTex);
	glUniform1i(pointlightShader->getUniform("normalTex"), 1);

	glUniform3f(
		pointlightShader->getUniform("cameraPos"),
		camera->getPosition().x, camera->getPosition().y, camera->getPosition().z
	);

	// Used to calculate texture coordinates for getting the depth and normals
	glUniform2f(pointlightShader->getUniform("pixelSize"),
		1.0f / width, 1.0f / height
	);

	// Save having to calculate this in the shader
	Matrix4 invViewProj = (projMatrix * viewMatrix).Inverse();
	invViewProj.bind(pointlightShader->getUniform("inverseProjView"));

	UpdateShaderMatrices();
	for (auto& light : pointLights) {
		light.bind(*this);
		sphere->Draw();
	}

	// Restore the state
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glCullFace(GL_BACK);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::combineBuffers() {
	BindShader(combineShader.get());
	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();
	UpdateShaderMatrices();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bufferColorTex);
	glUniform1i(combineShader->getUniform("diffuseTex"), 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, lightDiffuseTex);
	glUniform1i(combineShader->getUniform("diffuseLight"), 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, lightSpecularTex);
	glUniform1i(combineShader->getUniform("specularLight"), 2);

	quad->Draw();
}

void Renderer::UpdateScene(float dt)
{
	camera->update(dt);
}