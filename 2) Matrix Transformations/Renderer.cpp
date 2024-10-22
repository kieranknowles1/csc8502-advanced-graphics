#include "Renderer.h"

#include <gl/GL.h>

GLuint getUniform(Shader* shader, const char* name)
{
	GLuint program = shader->GetProgram();
	return glGetUniformLocation(program, name);
}

Renderer::Renderer(Window& parent)
	: OGLRenderer(parent)
	, scale(1.0f)
	, rotation(0.0f)
	, position(0, 0, 0)
	, shader(new Shader("MatrixVertex.glsl", "ColorFragment.glsl"))
	, triangle(Mesh::GenerateTriangle())
{
	SwitchToOrthographic();

	projLocation = getUniform(shader, "projMatrix");
	viewLocation = getUniform(shader, "viewMatrix");
	modelLocation = getUniform(shader, "modelMatrix");

	init = shader->LoadSuccess();
}

Renderer::~Renderer()
{
	delete triangle;
	delete shader;
}

void Renderer::RenderScene()
{
	CheckMatrix();

	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	BindShader(shader);
	// Bind our matrix uniform variables

	// Projection - the camera lens
	glUniformMatrix4fv(projLocation, 1, false, projMatrix.values);
	// View - Unused here, and set to the identity matrix
	glUniformMatrix4fv(viewLocation, 1, false, viewMatrix.values);

	// Draw the triangle in 3 different positions
	// Draw in reverse order to show why the depth buffer is important
	for (int i = 2; i >= 0; i--) {
		Vector3 pos = position;
		pos.z += 500 * i;
		pos.x -= 100 * i;
		pos.y -= 100 * i;

		modelMatrix = Matrix4::Translation(pos)
			* Matrix4::Rotation(rotation, Vector3(0, 1, 0))
			* Matrix4::Scale(Vector3(scale, scale, scale));

		// Model - the transformation of the object
		glUniformMatrix4fv(modelLocation, 1, false, modelMatrix.values);
		triangle->Draw();
	}
}

void Renderer::CheckMatrix()
{
	if (projType == ProjectionType::Orthographic) {
		SwitchToOrthographic();
	}
	else {
		SwitchToPerspective();
	}
}

void Renderer::SwitchToOrthographic()
{
	projType = ProjectionType::Orthographic;
	projMatrix = Matrix4::Orthographic(
		-1.0f, 1000.0f,
		width / 2.0f,
		-width / 2.0f,
		height / 2.0f,
		-height / 2.0f
	);
}

void Renderer::SwitchToPerspective()
{
	projType = ProjectionType::Perspective;
	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, fov);
}
