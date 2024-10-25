#pragma once

#include "Vector3.h"

class Plane
{
public:
	Plane() : normal(Vector3(0, 1, 0)), distance(0) {}
	Plane(const Vector3& normal, float distance, bool normalise = false);
	~Plane() {}

	void setNormal(const Vector3& normal) { this->normal = normal; }
	const Vector3& getNormal() const { return normal; }

	void setDistance(float distance) { this->distance = distance; }
	float getDistance() const { return distance; }

	bool sphereInPlane(const Vector3& position, float radius) const;

protected:
	Vector3 normal;
	float distance;
};

