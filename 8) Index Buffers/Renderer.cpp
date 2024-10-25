#include "Renderer.h"

Renderer::Renderer(Window& parent)
	: OGLRenderer(parent)
	, heightMap(new HeightMap(TEXTUREDIR"noise.png"))
	, camera(new Camera(-40, 270, 0, Vector3()))
	, shader(new Shader("WaterHeightVertex.glsl", "WaterHeightFragment.glsl"))
	, terrainTexture(SOIL_load_OGL_texture(TEXTUREDIR"Barren Reds.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS))
	, waterTexture(SOIL_load_OGL_texture(TEXTUREDIR"water.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS))
{
	if (!shader->LoadSuccess() || !terrainTexture || !waterTexture) {
		return;
	}

	camera->setPosition(heightMap->getSize() * Vector3(0.5, 2, 0.5));

	setTextureRepeating(terrainTexture, true);
	setTextureRepeating(waterTexture, true);
	projMatrix = Matrix4::Perspective(1, 10000, (float)width / (float)height, 45.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	init = true;
}

Renderer::~Renderer()
{
	delete heightMap;
	delete camera;
	delete shader;
	glDeleteTextures(1, &terrainTexture);
}

void Renderer::UpdateScene(float dt) {
	camera->update(dt);
	viewMatrix = camera->buildViewMatrix();

	time += dt;
}

void Renderer::RenderScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	BindShader(shader);
	UpdateShaderMatrices();

	glUniform1f(shader->getUniform("waterLevel"), waterLevel);

	glUniform1i(shader->getUniform("diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, terrainTexture);

	glUniform1i(shader->getUniform("waterTex"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, waterTexture);

	glUniform1f(shader->getUniform("time"), time);
	glUniform1f(shader->getUniform("waveAmplitude"), waveAmplitude);

	heightMap->Draw();
}