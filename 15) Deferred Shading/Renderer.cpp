#include "Renderer.h"

Renderer::Renderer(Window& parent)
	: OGLRenderer(parent)
	, resourceManager(std::make_unique<ResourceManager>())

	, sphere(Mesh::LoadFromMeshFile("Sphere.msh"))
	, quad(Mesh::GenerateQuad())
	, heightMap(std::make_unique<HeightMap>(TEXTUREDIR"noise.png"))

	, earthTex(resourceManager->getTextures().get({"Barren Reds.JPG", SOIL_FLAG_MIPMAPS, true}))
	, earthBump(resourceManager->getTextures().get({"Barren RedsDOT3.JPG", SOIL_FLAG_MIPMAPS, true}))
{
	auto heightMapSize = heightMap->getSize();
	camera = std::make_unique<Camera>(parent.GetKeyboard(), parent.GetMouse(), -45, 0, 0, heightMapSize * Vector3(0.5f, 5.0f, 0.5f));
}

Renderer::~Renderer()
{
}

void Renderer::RenderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	BindShader(sceneShader.get());
	UpdateShaderMatrices();

	heightMap->Draw();

}

void Renderer::UpdateScene(float dt)
{
	camera->update(dt);
	viewMatrix = camera->buildViewMatrix();
}