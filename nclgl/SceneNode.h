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
	void setParent(SceneNode* parent);

protected:
	// Called every frame BEFORE children are updated
	virtual void onUpdate(float dt) {}
	// Parents are drawn BEFORE their children
	virtual void drawSelf(const OGLRenderer& r);

	SceneNode* parent;
	Mesh* mesh;
	Matrix4 worldTransform;
	Matrix4 transform;
	Vector3 scale;
	Vector4 color;

	std::vector<SceneNode*> children;
};

