#include "SceneNode.h"

#include <cassert>
#include <algorithm>

SceneNode::SceneNode(std::shared_ptr<Mesh> mesh, Vector4 color)
    : parent(nullptr)
    , mesh(mesh)
    , color(color)
    , scale(Vector3(1, 1, 1))
    , distanceFromCamera(0)
    , boundingRadius(1)
    , isTransparent(false)
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

void SceneNode::drawSelf(OGLRenderer& r, bool shadowPass)
{
    if (!mesh)
        return;

    Matrix4 model = getWorldTransform();

    if (mesh->GetSubMeshCount() == 0) {
        auto mat = getMatrerial(0) ? getMatrerial(0) : &r.getDefaultMateriel();
        mat->bind(r, r.getDefaultMateriel(), shadowPass);
        color.bind(r.getCurrentShader()->getUniform("nodeColor"));
        model.bind(r.getCurrentShader()->getUniform("modelMatrix"));

        mesh->Draw();
    }
    else {
        for (int i = 0; i < mesh->GetSubMeshCount(); i++) {
            auto mat = getMatrerial(i) ? getMatrerial(i) : &r.getDefaultMateriel();
            mat->bind(r, r.getDefaultMateriel(), shadowPass);
            color.bind(r.getCurrentShader()->getUniform("nodeColor"));
            model.bind(r.getCurrentShader()->getUniform("modelMatrix"));
            mesh->DrawSubMesh(i);
        }
    }

    drawDebug(r);
}

void SceneNode::drawDebug(OGLRenderer& r) {
    const Shader* shader = r.getCurrentShader();
    if (r.getDebugSettings().drawBoundingBoxes) {
        // TODO: Use axis-aligned bounding box
        // This is currently not very accurate, but good enough for now
        float sqrt2 = sqrt(2);
        float radius = getBoundingRadius();
        Matrix4 transform = getWorldTransform()
            * Matrix4::Scale(Vector3(1.0 / scale.x, 1.0 / scale.y, 1.0 / scale.z)) // Undo scale to the node to get pre-scale bounding box
            * Matrix4::Scale(Vector3(radius * sqrt2, radius * sqrt2, radius * sqrt2)); // sqrt(2) gives an approximate fit, but it's not perfect
        transform.bind(shader->getUniform("modelMatrix"));
        // TODO: Reimplement debug cube
        //r.getDebugCube()->Draw();
    }
}

SceneNode::SceneNode(const SceneNode& s)
    : parent(nullptr)
    , materials(s.materials)
    , mesh(s.mesh)
    , color(s.color)
    , scale(s.scale)
    , transform(s.transform)
    , worldTransform(s.worldTransform)
    , boundingRadius(s.boundingRadius)
    , distanceFromCamera(s.distanceFromCamera)
    , tag(s.tag)
    {}

void SceneNode::copyChildrenFrom(const SceneNode& other)
{
	for (auto child : other.children) {
		addChild(child->deepCopy());
	}
}

SceneNode* SceneNode::deepCopy() const
{
    if (typeid(*this) != typeid(SceneNode))
		throw std::runtime_error("deepCopy not implemented for derived classes");

    SceneNode* copy = new SceneNode(*this);
    // Don't do this in the constructor, as derived classes
    // may need to know which child is which
    // Expose deepCopyChildren in case derived classes want to use it
    copy->copyChildrenFrom(*this);
    return copy;
}

SceneNode* SceneNode::getTaggedChild(const std::string& t)
{
	for (auto& child : children) {
		if (child->tag == t) {
			return child;
		}
	}
	return nullptr;
}