#include "Renderer.h"

Renderer::Renderer(Window& parent)
	: OGLRenderer(parent)
	, resourceManager(std::make_unique<ResourceManager>())
	, quad(Mesh::GenerateQuad())
	, heightMap(std::make_unique<HeightMap>(TEXTUREDIR "noise.png"))

	, lightShader(std::make_unique<Shader>("BumpVertex.glsl", "BumpFragment.glsl"))
	, reflectShader(std::make_unique<Shader>("ReflectVertex.glsl", "ReflectFragment.glsl"))
	, skyboxShader(std::make_unique<Shader>("SkyboxVertex.glsl", "SkyboxFragment.glsl"))
{
	waterTex = resourceManager->getTexture("water.tga", SOIL_FLAG_MIPMAPS);
	setTextureRepeating(waterTex->getId(), true);
	earthTex = resourceManager->getTexture("Barren Reds.JPG", SOIL_FLAG_MIPMAPS);
	setTextureRepeating(earthTex->getId(), true);
	earthBump = resourceManager->getTexture("Barren RedsDOT3.JPG", SOIL_FLAG_MIPMAPS);
	setTextureRepeating(earthBump->getId(), true);

	cubeMap = resourceManager->getCubeMap("rusted");


	Vector3 heightMapSize = heightMap->getSize();
	camera = std::make_unique<Camera>(
		parent.GetKeyboard(), parent.GetMouse(),
		-45, 0, 0,
		(heightMapSize * 0.5f) + Vector3(0, 500, 0)
	);

	light.reset(new Light(
		heightMapSize * Vector3(0.5, 1.5, 0.5),
		Vector3(1, 1, 1),
		heightMapSize.x
	));

	projMatrix = Matrix4::Perspective(1, 10000, (float)width / (float)height, 45.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// Interpolate across cube map boundaries
	// Disabled by default as older hardware didn't support it
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	waterRotate = 0.0f;
	waterCycle = 0.0f;
}

Renderer::~Renderer() {
}

void Renderer::RenderScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	drawSkybox();
	drawHeightmap();
	drawWater();
}

void Renderer::drawSkybox() {
	// Don't update the depth buffer, the sky is infinitely far away
	glDepthMask(GL_FALSE);

	BindShader(skyboxShader.get());
	UpdateShaderMatrices();

	quad->Draw();

	glDepthMask(GL_TRUE);
}

void Renderer::drawHeightmap() {
	BindShader(lightShader.get());
	// The water shader uses these
	modelMatrix.ToIdentity();
	textureMatrix.ToIdentity();
	UpdateShaderMatrices();


	glUniform1i(lightShader->getUniform("diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	earthTex->bind();

	glUniform1i(lightShader->getUniform("bumpTex"), 1);
	glActiveTexture(GL_TEXTURE1);
	earthBump->bind();

	light->bind(*this);
	glUniform3f(lightShader->getUniform("cameraPos"), camera->getPosition().x, camera->getPosition().y, camera->getPosition().z);

	heightMap->Draw();
}

void Renderer::drawWater() {
	BindShader(reflectShader.get());

	glUniform3f(reflectShader->getUniform("cameraPos"), camera->getPosition().x, camera->getPosition().y, camera->getPosition().z);

	glUniform1i(reflectShader->getUniform("diffuseTex"), 0);
	glUniform1i(reflectShader->getUniform("cubeTex"), 2);

	glActiveTexture(GL_TEXTURE0);
	waterTex->bind();

	glActiveTexture(GL_TEXTURE2);
	cubeMap->bind();;

	Vector3 heightSize = heightMap->getSize();
	modelMatrix =
		Matrix4::Translation(heightSize * 0.5f) *
		Matrix4::Scale(heightSize * 0.5f) *
		Matrix4::Rotation(90, Vector3(1, 0, 0));

	//textureMatrix =
	//	Matrix4::Translation(Vector3(waterCycle, 0, waterCycle)) *
	//	Matrix4::Scale(Vector3(10, 10, 10)) *
	//	Matrix4::Rotation(waterRotate, Vector3(0, 0, 1));

	UpdateShaderMatrices();
	heightMap->Draw();
	quad->Draw();
}

void Renderer::UpdateScene(float dt) {
	camera->update(dt);
	viewMatrix = camera->buildViewMatrix();

	waterRotate += dt * 2.0f;
	waterCycle += dt * 0.25f;
}
