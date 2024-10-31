#include "Renderer.h"

Renderer::Renderer(Window& parent)
	: OGLRenderer(parent)
	, resourceManager(std::make_unique<ResourceManager>())
	, heightMap(std::make_unique<HeightMap>(TEXTUREDIR "noise.png"))
	, heightMapTexture(resourceManager->getTextures().get({"Barren Reds.JPG", SOIL_FLAG_MIPMAPS, true}))
	, bumpMap(resourceManager->getTextures().get({"Barren RedsDOT3.JPG", SOIL_FLAG_MIPMAPS, true}))
	, shader(std::make_unique<Shader>("BumpVertex.glsl", "BumpFragment.glsl"))
{
	Vector3 heightMapSize = heightMap->getSize();
	camera = std::make_unique<Camera>(
		parent.GetKeyboard(), parent.GetMouse(),
		-45.0f, 0.0f, 0.0f,
		(heightMapSize * 0.5f) + Vector3(0, 500, 0)
	);
	light = std::make_unique<Light>(
		heightMapSize * Vector3(0.5f, 1.5f, 0.5f),
		Vector3(1.0f, 1.0f, 1.0f),
		heightMapSize.x * 0.5f, 2.0f
	);

	projMatrix = Matrix4::Perspective(
		1.0f, 10000.0f,
		(float)width / (float)height,
		45.0f
	);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
}

Renderer::~Renderer() {
}

void Renderer::RenderScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	BindShader(shader.get());
	UpdateShaderMatrices();

	glActiveTexture(GL_TEXTURE0);
	heightMapTexture->bind();
	glUniform1i(shader->getUniform("diffuseTex"), 0);

	glActiveTexture(GL_TEXTURE1);
	bumpMap->bind();
	glUniform1i(shader->getUniform("bumpTex"), 1);

	light->bind(*this);
	glUniform3f(
		shader->getUniform("cameraPos"),
		camera->getPosition().x,
		camera->getPosition().y,
		camera->getPosition().z
	);

	heightMap->Draw();
}

void Renderer::UpdateScene(float dt) {
	camera->update(dt);
	viewMatrix = camera->buildViewMatrix();
}
