#pragma once

#include "../nclgl/OGLRenderer.h"
#include "../nclgl/Camera.h"
#include "../nclgl/SceneNode.h"
#include "../nclgl/Mesh.h"
#include "../nclgl/Shader.h"

class Renderer : public OGLRenderer
{
public:
	Renderer(Window& parent);
	~Renderer() override;

	void UpdateScene(float dt) override;
	void RenderScene() override;

protected:
	void DrawNode(SceneNode* n);

	SceneNode* root;
	Camera* camera;
	Mesh* cube;
	Shader* shader;

	Mesh* plane;
	GLuint glass;
};

