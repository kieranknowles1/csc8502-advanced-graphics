#pragma once

#include "../nclgl/OGLRenderer.h"

class Renderer : public OGLRenderer {
public:
	Renderer(Window &parent);
	~Renderer() override;
	void RenderScene() override;

	void SetScale(float s) { scale = s; }
	void SetRotation(float r) { rotation = r; }
	void SetPosition(Vector3 p) { position = p; }

	void SwitchToOrthographic();
	void SwitchToPerspective();
protected:
	Mesh* triangle;
	Shader* shader;
	float scale;
	float rotation;
	Vector3 position;

	GLuint projLocation;
	GLuint viewLocation;
	GLuint modelLocation;
};
