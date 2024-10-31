#pragma once

#include <memory>

#include "../nclgl/OGLRenderer.h"
#include "../nclgl/HeightMap.h"
#include "../nclgl/Camera.h"
#include "../nclgl/Light.h"
#include "../nclgl/Shader.h"
#include "../nclgl/ResourceManager.h"

// Very simple node for demonstration purposes
struct BasicNode {
	// Takes ownership of the mesh
	BasicNode(Mesh* mesh, const Matrix4& transform = Matrix4())
		: mesh(mesh)
		, transform(transform)
	{}

	std::unique_ptr<Mesh> mesh;
	Matrix4 transform;
};

class Renderer : public OGLRenderer
{
public:
	constexpr static int ShadowSize = 2048;

	Renderer(Window &parent);
	~Renderer() override;

	void RenderScene() override;
	void UpdateScene(float dt) override;

protected:
	void drawShadowScene();
	void drawMainScene();

	std::unique_ptr<ResourceManager> resourceManager;

	GLuint shadowTex;
	GLuint shadowFBO;

	std::shared_ptr<Texture> sceneDiffuse;
	std::shared_ptr<Texture> sceneBump;
	float sceneTime;

	std::unique_ptr<Shader> sceneShader;
	std::unique_ptr<Shader> shadowShader;

	std::vector<BasicNode> meshes;

	std::unique_ptr<Camera> camera;
	std::unique_ptr<Light> light;
};
