#pragma once

#include "../nclgl/OGLRenderer.h"

enum class ProjectionType {
	Perspective,
	Orthographic
};

class Renderer : public OGLRenderer {
public:
	Renderer(Window &parent);
	~Renderer() override;
	void RenderScene() override;

	void SetScale(float s) { scale = s; }
	void SetRotation(float r) { rotation = r; }
	void SetPosition(Vector3 p) { position = p; }

	void SetFov(float value) { fov = value; }

	void SwitchToOrthographic();
	void SwitchToPerspective();
protected:
	// Update the projection matrix if it is dirty
	void CheckMatrix();

	Mesh* triangle;
	Shader* shader;
	float scale;
	float rotation;
	Vector3 position;

	GLuint projLocation;
	GLuint viewLocation;
	GLuint modelLocation;

	ProjectionType projType;
	float fov;
};
