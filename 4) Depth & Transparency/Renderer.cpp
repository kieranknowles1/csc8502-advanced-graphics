#include "Renderer.h"

Renderer::Renderer(Window& parent)
	: OGLRenderer(parent)
	, shader(new Shader("TexturedVertex.glsl", "TexturedFragment.glsl"))
	, useDepth(false)
	, useAlpha(false)
	, blendMode(0)
	, activeObject(0)
	, camera(new Camera(0, 0, 0, Vector3(0, 0, 0)))
{
	camera->setSpeed(1.0f);

	meshes[0] = Mesh::GenerateQuad();
	meshes[1] = Mesh::GenerateTriangle();

	textures[0] = SOIL_load_OGL_texture(
		TEXTUREDIR "brick.tga",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS
	);
	textures[1] = SOIL_load_OGL_texture(
		TEXTUREDIR "stainedglass.tga",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS
	);

	positions[0] = Vector3(0, 0, -5);
	positions[1] = Vector3(0, 0, -5);

	projMatrix = Matrix4::Perspective(
		1.0f, 100.0f,
		(float)width / (float)height, 45.0f
	);

	init = shader->LoadSuccess() && textures[0] && textures[1];
}

Renderer::~Renderer() {
	delete meshes[0];
	delete meshes[1];
	delete shader;
	glDeleteTextures(2, textures);
}

void Renderer::RenderScene() {
	float maxAniso = 0;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAniso);

	glClearColor(0.2, 0.2, 0.2, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	BindShader(shader);
	UpdateShaderMatrices();
	glUniform1i(shader->getUniform("diffuseTex"), 0);
	glUniform1f(shader->getUniform("alphaThreshold"), alphaThreshold);
	glActiveTexture(GL_TEXTURE0);
	for (GLuint i = 0; i < 2; i++) {
		glUniformMatrix4fv(shader->getUniform("modelMatrix"), 1, false,
			(float*)&Matrix4::Translation(positions[i])
		);
		glBindTexture(GL_TEXTURE_2D, textures[i]);
		// Use anisotropic filtering to keep textures from being
		// blurry at an oblique angle.
		// This is a very cheap operation, so put it as high as possible.
		// Only real impact is on memory bandwidth.
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAniso);
		meshes[i]->Draw();
	}
}

void Renderer::UpdateScene(float dt)
{
	camera->update(dt);
	viewMatrix = camera->buildViewMatrix();
}

void Renderer::ToggleObject()
{
	activeObject = (activeObject == 0) ? 1 : 0;
}

static void toggleGl(GLenum flag, bool* value) {
	*value = !*value;
	if (*value) {
		glEnable(flag);
	}
	else {
		glDisable(flag);
	}
}

void Renderer::ToggleDepth()
{
	toggleGl(GL_DEPTH_TEST, &useDepth);
}

void Renderer::ToggleAlphaBlend()
{
	toggleGl(GL_BLEND, &useAlpha);
}

void Renderer::ToggleBlendMode()
{
	blendMode = (blendMode + 1) % 4;
	switch (blendMode) {
	case 0: glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); break;
	case 1: glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR); break;
	case 2: glBlendFunc(GL_ONE, GL_ZERO); break;
	case 3: glBlendFunc(GL_SRC_ALPHA, GL_ONE); break;
	}
}

void Renderer::MoveObject(float by)
{
	positions[activeObject].z += by;
}
