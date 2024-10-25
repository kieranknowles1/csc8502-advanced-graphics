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
	detachFromParent();
	for (auto& child : children)
	{
		// Clear the child's parent so it doesn't try to detach itself
		child->parent = nullptr;
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

void SceneNode::detachFromParent()
{
	if (parent == nullptr)
		return;

	auto self = std::find(parent->children.begin(), parent->children.end(), this);
	parent->children.erase(self);

	parent = nullptr;
}

void SceneNode::setParent(SceneNode* newParent)
{
	if (parent != nullptr) {
		detachFromParent();
	}

	assert(newParent != this && "Cannot set parent to self");
	assert((newParent == nullptr || !newParent->isChildOf(this)) && "Infinite node loop detected");
	parent = newParent;
}

void SceneNode::addChild(SceneNode* s)
{
	s->setParent(this);
	children.push_back(s);
}

void SceneNode::update(float dt)
{
	// Scale needs to be applied after rotation (which may happen in onUpdate)
	// otherwise we'll get weird results
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
