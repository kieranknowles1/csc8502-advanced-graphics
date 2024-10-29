#include "Renderer.h"

Renderer::Renderer(Window& parent)
	: OGLRenderer(parent)
	, resourceManager(std::make_unique<ResourceManager>())
	, heightMap(std::make_unique<HeightMap>(TEXTUREDIR "noise.png"))
	, heightMapTexture(resourceManager->getTexture("Barren Reds.JPG", SOIL_FLAG_MIPMAPS))
	, shader(std::make_unique<Shader>("PerPixelVertex.glsl", "PerPixelFragment.glsl"))
{
	// TODO: This should be part of the resource manager
	setTextureRepeating(heightMapTexture->getId(), true);

	Vector3 heightMapSize = heightMap->getSize();
	camera = std::make_unique<Camera>(
		parent.GetKeyboard(), parent.GetMouse(),
		-45.0f, 0.0f, 0.0f,
		(heightMapSize * 0.5f) + Vector3(0, 500, 0)
	);
	light = std::make_unique<Light>(
		heightMapSize * Vector3(0.5f, 1.5f, 0.5f),
		Vector4(1.0f, 1.0f, 1.0f, 1.0f),
		heightMapSize.x * 0.5f
	);

	projMatrix = Matrix4::Perspective(
		1.0f, 10000.0f,
		(float)width / (float)height,
		45.0f
	);

	glEnable(GL_DEPTH_TEST);
}

Renderer::~Renderer() {
}

void Renderer::RenderScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	BindShader(shader.get());
	UpdateShaderMatrices();

	heightMapTexture->bind();

	light->bind(*this);
	glUniform3fv(
		shader->getUniform("cameraPos"),
		1, (float*)&camera->getPosition()
	);

	heightMap->Draw();
}

void Renderer::UpdateScene(float dt) {
	camera->update(dt);
	viewMatrix = camera->buildViewMatrix();
}