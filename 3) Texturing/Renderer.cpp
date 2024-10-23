#include "Renderer.h"

Renderer::Renderer(Window& window)
	: OGLRenderer(window)
	, shader(new Shader("TexturedVertex.glsl", "TexturedFragment.glsl"))
	, triangle(Mesh::GenerateQuad())
	, filterEnabled(true)
	, repeatEnabled(false)
{
	texture = SOIL_load_OGL_texture(
		TEXTUREDIR "brick.tga",
		SOIL_LOAD_AUTO, // Determine channels automatically
		SOIL_CREATE_NEW_ID, // Allocate a new texture
		SOIL_FLAG_MIPMAPS // Generate mipmaps
	);
	blendTexture = SOIL_load_OGL_texture(
		TEXTUREDIR "Barren Reds.JPG",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS
	);

	init = shader->LoadSuccess() && texture && blendTexture;
}

Renderer::~Renderer() {
	delete shader;
	delete triangle;
	glDeleteTextures(1, &texture); // Free our texture
}

void Renderer::setTexture(GLenum unit, const char* uniformName, GLuint texture)
{
	// The GL_TEXTURE enum is contiguous, so we can subtract GL_TEXTURE0
	// to get the index
	int unitNum = unit - GL_TEXTURE0;
	// Set the shader's uniform to the unit number
	glUniform1i(glGetUniformLocation(shader->GetProgram(), uniformName), unitNum);
	// Future texture functions will run on this unit
	glActiveTexture(unit);
	glBindTexture(GL_TEXTURE_2D, texture);
}

void Renderer::RenderScene() {
	glClearColor(0.2, 0.2, 0.2, 1.0);
	// Clear the depth buffer, otherwise everything would be discarded
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	BindShader(shader);
	UpdateShaderMatrices();

	// Bind our texture, pass a uniform saying that it's in unit 0
	setTexture(GL_TEXTURE0, "diffuseTex", texture);
	setTexture(GL_TEXTURE1, "blendTex", blendTexture);
	triangle->Draw();
}

void Renderer::UpdateTextureMatrix(float rotation, float scale, Vector2 position)
{
	Vector3 pushVec(0.5, 0.5, 0.0);

	// Translate our texture by half its size so that we rotate
	// around the middle
	Matrix4 push = Matrix4::Translation(pushVec);
	Matrix4 pop = Matrix4::Translation(-pushVec);
	Matrix4 rotationMat = Matrix4::Rotation(rotation, Vector3(0, 0, 1));
	Matrix4 scaleMat = Matrix4::Scale(Vector3(scale, scale, 1));

	Matrix4 translation = Matrix4::Translation(Vector3(position.x, position.y, 0));

	textureMatrix = push * rotationMat * scaleMat * pop * translation;
}

void Renderer::ToggleRepeating()
{
	repeatEnabled = !repeatEnabled;
	setTextureRepeating(texture, repeatEnabled);
	setTextureRepeating(blendTexture, repeatEnabled);
}

void Renderer::ToggleFiltering()
{
	// TODO: Implement
}
