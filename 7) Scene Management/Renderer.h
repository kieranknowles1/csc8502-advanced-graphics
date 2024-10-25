#pragma once

#include "../nclgl/OGLRenderer.h"
#include "../nclgl/Camera.h"
#include "../nclgl/SceneNode.h"
#include "../nclgl/Mesh.h"
#include "../nclgl/Shader.h"
#include "../nclgl/Frustum.h"

class Renderer : public OGLRenderer
{
public:
	Renderer(Window& parent);
	~Renderer() override;

	void UpdateScene(float dt) override;
	void RenderScene() override;

	void toggleFrustumLock() { lockFrustum = !lockFrustum; }

protected:
	void DrawNode(SceneNode* n);

	bool lockFrustum = false;

	SceneNode* root;
	Camera* camera;
	Mesh* cube;
	Shader* shader;

	Mesh* plane;
	GLuint glass;

	Frustum viewFrustum;
	std::vector<SceneNode*> transparentNodes;
	std::vector<SceneNode*> opaqueNodes;

	void buildNodeLists(SceneNode* from);
	void sortNodeLists();
	void clearNodeLists();

	void drawNodes(const std::vector<SceneNode*>& nodes);
};

