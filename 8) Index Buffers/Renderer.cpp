#include "Renderer.h"

Renderer::Renderer(Window& parent)
	: OGLRenderer(parent)
	, heightMap(new HeightMap(TEXTUREDIR"noise.png"))
	, camera(new Camera(-40, 270, 0, Vector3()))
	, shader(new Shader("TexturedVertex.glsl", "TexturedFragment.glsl"))
	, terrainTexture(SOIL_load_OGL_texture(TEXTUREDIR"Barren Reds.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS))
{
	if (!shader->LoadSuccess() || !terrainTexture) {
		return;
	}

	camera->setPosition(heightMap->getSize() * Vector3(0.5, 2, 0.5));

	setTextureRepeating(terrainTexture, true);
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
}

void Renderer::RenderScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	BindShader(shader);
	UpdateShaderMatrices();

	glUniform1i(shader->getUniform("diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, terrainTexture);

	heightMap->Draw();
}