#include "Renderer.h"

Renderer::Renderer(Window& parent)
	: OGLRenderer(parent)
	, camera(new Camera())
	, resourceManager(new ResourceManager())
{
	camera->setSpeed(10.0f);
	camera->setPosition(Vector3(0, 2, 5));

	projMatrix = Matrix4::Perspective(
		1.0f, 10000.0f,
		(float)width / (float)height,
		45.0f
	);

	root = new SceneNode();

	for (int i = 0; i < 10; i++) {
		SkeletonAnim* node = new SkeletonAnim(resourceManager, "Role_T.msh", "Role_T.anm", "Role_T.mat");
		node->setTransform(Matrix4::Translation(Vector3(0, 2 * i, i * -2)));
		root->addChild(node);
	}

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	init = true;
}

Renderer::~Renderer()
{
	delete camera;
	delete resourceManager;
}

void Renderer::UpdateScene(float dt)
{
	camera->update(dt);
	viewMatrix = camera->buildViewMatrix();

	root->update(dt);
}

void Renderer::RenderScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	drawNode(*root);
}

void Renderer::drawNode(SceneNode& node) {
	node.drawSelf(*this);
	for (auto child = node.childrenBegin(); child < node.childrenEnd(); child++) {
		drawNode(**child);
	}
}