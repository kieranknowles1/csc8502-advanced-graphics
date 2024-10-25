#include "Frustum.h"

#include "SceneNode.h"
#include "Matrix4.h"

bool Frustum::inFrustum(const SceneNode& node) const {
	for (int p = 0; p < 6; p++) {
		if (!planes[p].sphereInPlane(node.getWorldTransform().GetPositionVector(), node.getBoundingRadius())) {
			return false;
		}
	}
	return true;
}

void Frustum::fillFromMatrix(const Matrix4& mat) {
	// Extract each axis from the matrix
	// then add/subtract to tilt the normals
	Vector3 xaxis = Vector3(mat.values[0], mat.values[4], mat.values[8]);
	Vector3 yaxis = Vector3(mat.values[1], mat.values[5], mat.values[9]);
	Vector3 zaxis = Vector3(mat.values[2], mat.values[6], mat.values[10]);
	Vector3 waxis = Vector3(mat.values[3], mat.values[7], mat.values[11]);

	// Right
	planes[0] = Plane(
		waxis - xaxis,
		mat.values[15] - mat.values[12],
		true
	);

	// Left
	planes[1] = Plane(
		waxis + xaxis,
		mat.values[15] + mat.values[12],
		true
	);

	// Bottom
	planes[2] = Plane(
		waxis + yaxis,
		mat.values[15] + mat.values[13],
		true
	);

	// Top
	planes[3] = Plane(
		waxis - yaxis,
		mat.values[15] - mat.values[13],
		true
	);

	// Far
	planes[4] = Plane(
		waxis - zaxis,
		mat.values[15] - mat.values[14],
		true
	);

	// Near
	planes[5] = Plane(
		waxis + zaxis,
		mat.values[15] + mat.values[14],
		true
	);
}