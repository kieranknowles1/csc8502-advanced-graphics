#include "Renderer.h"

Renderer::Renderer(Window & parent)
	: OGLRenderer(parent)
	, triangle(Mesh::GenerateTriangle())
	, basicShader(new Shader("BasicVertex.glsl", "ColorFragment.glsl"))
{
	init = basicShader->LoadSuccess();
}

Renderer::~Renderer() {
	delete triangle;
	delete basicShader;
}

void Renderer::RenderScene() {
	// Clear the screen with dark grey
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// Draw the triangle using our basic shader
	BindShader(basicShader);
	triangle->Draw();
}