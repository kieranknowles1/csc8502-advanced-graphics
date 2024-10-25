#pragma once

#include "Matrix4.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Mesh.h"
#include <vector>

// A node in the scene graph. Should not be reused for multiple objects.
class SceneNode
{
public:
	// Copy this node and all its children
	// Overrides should set themselves up as close to the original as possible
	virtual SceneNode* deepCopy() const;

	SceneNode(Mesh* mesh = nullptr, Vector4 color = Vector4(1, 1, 1, 1));
	virtual ~SceneNode();

	// Set the local transform of this node
	void setTransform(const Matrix4& matrix) { transform = matrix; }
	const Matrix4& getTransform() const { return transform; }
	Matrix4 getWorldTransform() const { return worldTransform; }

	Vector4 getColor() const { return color; }
	void setColor(const Vector4& c) { color = c; }

	Vector3 getScale() const { return scale; }
	void setScale(const Vector3& s) { scale = s; }

	Mesh* getMesh() const { return mesh; }
	void setMesh(Mesh* mesh) { this->mesh = mesh; }

	void addChild(SceneNode* s);
	void update(float dt);
	// Draw this node and all its children
	void draw(const OGLRenderer& r);

	bool isChildOf(SceneNode* other);

	// Set the parent of this node
	// If the node already has a parent, it will be moved to the new parent
	// Setting the parent to nullptr will remove the node from the scene graph,
	// but not delete it
	void setParent(SceneNode* newParent);

	virtual const std::string getName() const { return "SceneNode"; }

protected:
	// Called every frame BEFORE children are updated
	virtual void onUpdate(float dt) {}
	// Parents are drawn BEFORE their children
	virtual void drawSelf(const OGLRenderer& r);

	Mesh* mesh;
	Matrix4 worldTransform;
	Matrix4 transform;
	Vector3 scale;
	Vector4 color;

	std::vector<SceneNode*> children;

	// Don't make this public, if we want to make
	// a copy from a derived pointer, deepCopy is
	// needed to make sure the right type is created

	// Copies the parent, but not the children
	SceneNode(const SceneNode& s);

	SceneNode* parent;

private:
	// Detach this node from its parent,
	// setting parent to nullptr and removing it from the parent's children
	// Does nothing if the node has no parent
	void detachFromParent();
};

