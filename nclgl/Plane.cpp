#include "Plane.h"

Plane::Plane(const Vector3& normal, float distance, bool normalise)
{
	if (normalise) {
		float length = std::sqrt(Vector3::Dot(normal, normal));
		this->normal = normal / length;
		this->distance = distance / length;
	}
	else {
		this->normal = normal;
		this->distance = distance;
	}
}

bool Plane::sphereInPlane(const Vector3& position, float radius) const
{
	if (Vector3::Dot(position, normal) + distance <= -radius) {
		return false;
	}
	return true;
}