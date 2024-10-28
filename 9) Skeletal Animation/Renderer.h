#pragma once

#include <memory>

#include "../nclgl/OGLRenderer.h"
#include "../nclgl/Camera.h"
#include "../nclgl/Mesh.h"
#include "../nclgl/MeshAnimation.h"
#include "../nclgl/MeshMaterial.h"
#include "../nclgl/ResourceManager.h"
#include "../nclgl/SkeletonAnim.h"

class Renderer : public OGLRenderer
{
public:
	Renderer(Window& parent);
	~Renderer() override;

	void RenderScene() override;
	void UpdateScene(float dt) override;

protected:
	void drawNode(SceneNode& n);

	SceneNode* root;

	Camera* camera;
	ResourceManager* resourceManager;
};
