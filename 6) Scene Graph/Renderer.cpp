#include "Renderer.h"

#include "../nclgl/CubeBot.h"

Renderer::Renderer(Window& parent)
	: OGLRenderer(parent)
	, root(new SceneNode())
	, cube(Mesh::LoadFromMeshFile("OffsetCubeY.msh"))
	, camera(new Camera())
	, shader(new Shader("SceneVertex.glsl", "SceneFragment.glsl"))
{
	if (!shader->LoadSuccess() || !cube)
		return;

	projMatrix = Matrix4::Perspective(
		1.0f, 1000.0f,
		(float)width / (float)height,
		45.0f
	);

	camera->setPosition(Vector3(0, 30, 175));

	SceneNode* bot = new CubeBot(cube);
	root->addChild(bot);

	for (int i = 0; i < 10; i++) {
		bot = bot->deepCopy();
		bot->setTransform(bot->getTransform() * Matrix4::Translation(Vector3(0, 0, -100)));
		root->addChild(bot);
	}

	init = true;

	glEnable(GL_DEPTH_TEST);
}

Renderer::~Renderer()
{
	delete root;
	delete cube;
	delete camera;
	delete shader;
}

void Renderer::UpdateScene(float dt)
{
	camera->update(dt);
	viewMatrix = camera->buildViewMatrix();
	root->update(dt);
}

void Renderer::RenderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	BindShader(shader);
	UpdateShaderMatrices();

	glUniform1i(shader->getUniform("diffuseTex"), 1);

	DrawNode(root);
}

void Renderer::DrawNode(SceneNode* node)
{
	node->draw(*this);
}