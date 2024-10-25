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

	for (int i = 0; i < 4; i++) {
		SceneNode* planeNode = new SceneNode(plane, Vector4(1, 1, 1, 0.9f));
		planeNode->setTransform(Matrix4::Translation(Vector3(0, 0, 50 * (i+1))) * Matrix4::Scale(Vector3(100, 100, 1)));
		planeNode->setTexture(glass);
		planeNode->setBoundingRadius(100);
		root->addChild(planeNode);
	}

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
	if (!lockFrustum)
		viewFrustum.fillFromMatrix(projMatrix * viewMatrix);
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
	buildNodeLists(node);
	sortNodeLists();

	drawNodes(opaqueNodes);
	drawNodes(transparentNodes);

	clearNodeLists();
}

void Renderer::buildNodeLists(SceneNode* from)
{
	if (viewFrustum.inFrustum(*from)) {
		Vector3 dir = from->getWorldTransform().GetPositionVector() - camera->getPosition();
		from->setCameraDistance(Vector3::Dot(dir, dir));

		if (from->getColor().w < 1.0f) {
			transparentNodes.push_back(from);
		}
		else {
			opaqueNodes.push_back(from);
		}
	}

	for (auto& child = from->childrenBegin(); child != from->childrenEnd(); child++) {
		buildNodeLists(*child);
	}
}

void Renderer::sortNodeLists()
{
	// TODO: Implement
}

void Renderer::clearNodeLists()
{
	opaqueNodes.clear();
	transparentNodes.clear();
}

void Renderer::drawNodes(const std::vector<SceneNode*>& nodes)
{
	for (auto& node : nodes) {
		node->drawSelf(*this);
	}
}