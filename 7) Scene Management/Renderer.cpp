#include "Renderer.h"

#include "../nclgl/CubeBot.h"

Renderer::Renderer(Window& parent)
	: OGLRenderer(parent)
	, root(new SceneNode())
	, cube(Mesh::LoadFromMeshFile("OffsetCubeY.msh"))
	, camera(new Camera())
	, shader(new Shader("SceneVertex.glsl", "SceneFragment.glsl"))
	, plane(Mesh::GenerateQuad())
	, glass(SOIL_load_OGL_texture(TEXTUREDIR "stainedglass.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0))
{
	if (!shader->LoadSuccess() || !cube || !glass)
		return;

	projMatrix = Matrix4::Perspective(
		1.0f, 1000.0f,
		(float)width / (float)height,
		45.0f
	);

	camera->setPosition(Vector3(0, 30, 175));

	SceneNode* bot = new CubeBot(cube);
	root->addChild(bot);

	SceneNode* planeNode = new SceneNode(plane);
	planeNode->setTransform(Matrix4::Translation(Vector3(0, 0, 50)) * Matrix4::Scale(Vector3(100, 100, 1)));
	planeNode->setTexture(glass);
	root->addChild(planeNode);

	for (int i = 0; i < 10; i++) {
		bot = bot->deepCopy();
		bot->setTransform(bot->getTransform() * Matrix4::Translation(Vector3(0, 0, -100)));
		bot->setScale(bot->getScale() * 1.2f);
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
	delete plane;
	glDeleteTextures(1, &glass);
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
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	BindShader(shader);
	UpdateShaderMatrices();

	glUniform1i(shader->getUniform("diffuseTex"), 1);

	DrawNode(root);
}

void Renderer::DrawNode(SceneNode* node)
{
	node->draw(*this);
}