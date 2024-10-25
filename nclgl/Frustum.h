#pragma once

#include "Plane.h"

// Forward declaration to avoid pulling in too many headers
class SceneNode;
class Matrix4;

class Frustum
{
public:
	Frustum() {}
	~Frustum() {}

	// Fill the frustum with planes extracted from the view-projection matrix
	void fillFromMatrix(const Matrix4& viewProj);
	bool inFrustum(const SceneNode& n) const;

protected:
	Plane planes[6];
};

