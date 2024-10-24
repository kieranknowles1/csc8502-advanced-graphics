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
	worldTransform = parent ? parent->worldTransform * transform * Matrix4::Scale(scale) : transform;
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
	// it isn't inherited by children
	// Currently needed as we're using scaled primitives
	Matrix4 model = getWorldTransform();
	model.bind(shader->getUniform("modelMatrix"));
	color.bind(shader->getUniform("nodeColor"));
	glUniform1i(shader->getUniform("useTexture"), 0);

	mesh->Draw();
}

SceneNode::SceneNode(const SceneNode& s)
	: parent(nullptr)
	, mesh(s.mesh)
	, color(s.color)
	, scale(s.scale)
	, transform(s.transform)
	, worldTransform(s.worldTransform)
	{}

SceneNode* SceneNode::deepCopy() const
{
	SceneNode* copy = new SceneNode(*this);
	// Don't do this in the constructor, as derived classes
	// may need to know which child is which
	for (auto child : children) {
		std::cout << "Child: " << child->getName() << std::endl;
		copy->addChild(child->deepCopy());
	}
	return copy;
}
