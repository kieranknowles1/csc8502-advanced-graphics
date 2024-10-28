#pragma once

#include <memory>

#include "../nclgl/OGLRenderer.h"
#include "../nclgl/Camera.h"
#include "../nclgl/Mesh.h"
#include "../nclgl/MeshAnimation.h"
#include "../nclgl/MeshMaterial.h"
#include "../nclgl/ResourceManager.h"

class Renderer : public OGLRenderer
{
public:
	Renderer(Window& parent);
	~Renderer() override;

	void RenderScene() override;
	void UpdateScene(float dt) override;

protected:
	Camera* camera;
	Mesh* mesh;
	Shader* shader;
	MeshAnimation* anim;
	MeshMaterial* material;
	ResourceManager* resourceManager;
	std::vector<std::shared_ptr<ManagedTexture>> matTextures;

	int currentFrame;
	// Time until next frame
	float frameTime;
};
