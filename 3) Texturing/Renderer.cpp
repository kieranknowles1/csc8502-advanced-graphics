#include "Renderer.h"

Renderer::Renderer(Window& window)
	: OGLRenderer(window)
	, shader(new Shader("TexturedVertex.glsl", "TexturedFragment.glsl"))
	, triangle(Mesh::GenerateTriangle())
	, filterEnabled(true)
	, repeatEnabled(false)
{
	texture = SOIL_load_OGL_texture(
		TEXTUREDIR "brick.tga",
		SOIL_LOAD_AUTO, // Determine channels automatically
		SOIL_CREATE_NEW_ID, // Allocate a new texture
		SOIL_FLAG_MIPMAPS // Generate mipmaps
	);

	init = shader->LoadSuccess() && texture;
}

Renderer::~Renderer() {
	delete shader;
	delete triangle;
	glDeleteTextures(1, &texture); // Free our texture
}

void Renderer::RenderScene() {
	glClearColor(0.2, 0.2, 0.2, 1.0);
	// Clear the depth buffer, otherwise everything would be discarded
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	BindShader(shader);
	UpdateShaderMatrices();

	// Bind our texture, pass a uniform saying that it's in unit 0
	glUniform1i(glGetUniformLocation(
		shader->GetProgram(), "diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0); // Future texture functions will run on this unit
	glBindTexture(GL_TEXTURE_2D, texture);
	triangle->Draw();
}

void Renderer::UpdateTextureMatrix(float rotation, float scale)
{
	Vector3 pushVec(0.5, 0.5, 0.0);

	// Translate our texture by half its size so that we rotate
	// around the middle
	Matrix4 push = Matrix4::Translation(pushVec);
	Matrix4 pop = Matrix4::Translation(-pushVec);
	Matrix4 rotationMat = Matrix4::Rotation(rotation, Vector3(0, 0, 1));
	// Use 1/scale to make the texture smaller as scale decreases
	Matrix4 scaleMat = Matrix4::Scale(Vector3(1/scale, 1/scale, 1));

	textureMatrix = push * rotationMat * scaleMat * pop;
}

void Renderer::ToggleRepeating()
{
	repeatEnabled = !repeatEnabled;
	setTextureRepeating(texture, repeatEnabled);
}

void Renderer::ToggleFiltering()
{
	// TODO: Implement
}
