#pragma once

#include "../nclgl/Camera.h"
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
	void UpdateScene(float dt) override;

	void SetScale(float s) { scale = s; }
	void SetRotation(float r) { rotation = r; }
	void SetPosition(Vector3 p) { position = p; }

	void SetFov(float value) { fov = value; }

	void SwitchToOrthographic();
	void SwitchToPerspective();
protected:
	// Update the projection matrix if it is dirty
	void CheckMatrix();

	Camera* camera;

	Mesh* triangle;
	Shader* shader;
	float scale;
	float rotation;
	Vector3 position;

	ProjectionType projType;
	float fov;
};
