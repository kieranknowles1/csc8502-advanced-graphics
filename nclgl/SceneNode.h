#pragma once

#include <vector>

#include "Matrix4.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Mesh.h"
#include "Materiel.h"

// A node in the scene graph. Should not be reused for multiple objects.
class SceneNode
{
public:
	static bool compareByCameraDistance(SceneNode* a, SceneNode* b) {
		return a->distanceFromCamera < b->distanceFromCamera;
	}

	// Copy this node and all its children
	// Overrides should set themselves up as close to the original as possible
	virtual SceneNode* deepCopy() const;

	SceneNode(std::shared_ptr<Mesh> mesh = nullptr, Vector4 color = Vector4(1, 1, 1, 1));
	virtual ~SceneNode();

	// Set the local transform of this node
	void setTransform(const Matrix4& matrix) { transform = matrix; }
	const Matrix4& getTransform() const { return transform; }
	Matrix4 getWorldTransform() const { return worldTransform; }

	Vector4 getColor() const { return color; }
	void setColor(const Vector4& c) { color = c; }

	Vector3 getScale() const { return scale; }
	void setScale(const Vector3& s) { scale = s; }

	std::shared_ptr<Mesh> getMesh() const { return mesh; }
	void setMesh(std::shared_ptr<Mesh> mesh) { this->mesh = mesh; }

	// Add a child to this node. This node will take ownership of the child
	// If the child already has a parent, it will be moved to this node
	void addChild(SceneNode* s);
	void update(float dt);

	bool isChildOf(SceneNode* other);

	// Set the parent of this node
	// If the node already has a parent, it will be moved to the new parent
	// Setting the parent to nullptr will remove the node from the scene graph,
	// but not delete it
	// DOES NOT add the node to the parent's children, use addChild for that
	void setParent(SceneNode* newParent);

	float getBoundingRadius() const {
		// FIXME: This is not adjusted by parent scale. A bounding box would be better
		float maxScale = std::max(std::abs(scale.x), std::max(std::abs(scale.y), std::abs(scale.z)));
		return boundingRadius * maxScale;
	}
	void setBoundingRadius(float r) { boundingRadius = r; }

	float getCameraDistance() const { return distanceFromCamera; }
	void setCameraDistance(float d) { distanceFromCamera = d; }

	virtual const std::string getName() const { return "SceneNode"; }

	// Parents are drawn BEFORE their children
	virtual void drawSelf(OGLRenderer& r);
	void drawDebug(OGLRenderer& r);

	bool getIsTransparent() const { return isTransparent; }
	void setIsTransparent(bool t) { isTransparent = t; }

	bool isEnabled() const { return enabled; }
	void setEnabled(bool e) { enabled = e; }

	void setMateriel(const Materiel& m) {
		materials.clear();
		materials.push_back(m);
	}
	void setMateriels(const std::vector<Materiel>& m) {
		materials = m;
	}

	using SceneNodeIterator = std::vector<SceneNode*>::iterator;
	SceneNodeIterator childrenBegin() { return children.begin(); }
	SceneNodeIterator childrenEnd() { return children.end(); }

	void setTag(const std::string& t) { tag = t; }
	const std::string& getTag() const { return tag; }

	// Returns the child with the given tag, or nullptr if not found
	// Undefined behaviour if multiple children have the same tag
	// Does not recurse into children
	SceneNode* getTaggedChild(const std::string& t);
protected:
	// Called every frame BEFORE children are updated
	virtual void onUpdate(float dt) {}

	std::shared_ptr<Mesh> mesh;
	Matrix4 worldTransform;
	Matrix4 transform;
	Vector3 scale;
	Vector4 color;

	float boundingRadius;
	float distanceFromCamera;

	bool isTransparent = false;
	bool enabled = true;

	std::vector<Materiel> materials;
	Materiel* getMatrerial(int i) {
		if (i < 0 || i >= materials.size())
			return nullptr;
		return &materials[i];
	}

	std::vector<SceneNode*> children;

	// Don't make this public, if we want to make
	// a copy from a derived pointer, deepCopy is
	// needed to make sure the right type is created

	// Copies the parent, but not the children
	SceneNode(const SceneNode& s);
	void copyChildrenFrom(const SceneNode& from);

	SceneNode* parent;

	std::string tag = "";

private:
	// Detach this node from its parent,
	// setting parent to nullptr and removing it from the parent's children
	// Does nothing if the node has no parent
	void detachFromParent();
};

