#include "SceneNode.h"

#include <cassert>

SceneNode::SceneNode(Mesh* mesh, Vector4 color)
	: parent(nullptr)
	, mesh(mesh)
	, color(color)
	, scale(Vector3(1, 1, 1))
{}

SceneNode::~SceneNode()
{
	for (auto& child : children)
	{
		delete child;
	}
}

bool SceneNode::isChildOf(SceneNode* other)
{
	if (this == other)
		return true;
	if (parent == nullptr)
		return false;
	return parent->isChildOf(other);
}

void SceneNode::setParent(SceneNode* parent)
{
	assert(parent != this && "Cannot set parent to self");
	assert(this->parent == nullptr && "Node already has a parent");
	assert(!isChildOf(parent) && "Infinite node loop detected");
	this->parent = parent;
}

void SceneNode::addChild(SceneNode* s)
{
	s->setParent(this);
	children.push_back(s);
}

void SceneNode::update(float dt)
{
	worldTransform = parent ? parent->worldTransform * transform : transform;
	onUpdate(dt);

	for (auto& child : children)
	{
		child->update(dt);
	}
}

void SceneNode::draw(const OGLRenderer& r)
{
	drawSelf(r);

	for (auto& child : children)
	{
		child->draw(r);
	}
}

void SceneNode::drawSelf(const OGLRenderer& r)
{
	if (!mesh)
		return;

	const Shader* shader = r.getCurrentShader();
	// TODO: Have scale as part of the world transform, otherwise
	// it isn't inherited by children
	// Currently needed as we're using scaled primitives
	Matrix4 model = getWorldTransform() * Matrix4::Scale(scale);
	model.bind(shader->getUniform("modelMatrix"));
	color.bind(shader->getUniform("nodeColor"));
	glUniform1i(shader->getUniform("useTexture"), 0);

	mesh->Draw();
}