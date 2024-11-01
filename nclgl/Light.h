#pragma once

#include "SceneNode.h"

class OGLRenderer;

class Light : public SceneNode
{
public:
	static const std::string PositionUniform;
	static const std::string ColorUniform;
	static const std::string RadiusUniform;
	static const std::string AttenuationUniform;
	static const std::string FOVUniform;
	static const std::string forwardUniform;

	Light(float radius, float attenuation = 2.0f)
		: radius(radius), attenuation(attenuation) {}

	~Light(void) {}

	// Bind this light's uniforms to the active shader
	void bind(const OGLRenderer& renderer) const;

	void setRadius(float r) { radius = r; }
	float getRadius() const { return radius; }

	void setFov(float f) {
		if (f > 180.0f) f = 180.0f;
		if (f < 0.0f) f = 0.0f;
		fov = f;
	}
	float getFov() const { return fov; }

	void setFacing(const Vector3& f) { facing = f; }
	Vector3 getFacing() const { return facing; }

protected:
	float radius;
	// How quickly the light fades with distance. 2.0 is a good default
	// Too small and there will be a sharp cut-off, too large and the light will be too weak
	// See https://www.desmos.com/calculator/ecqboyzch2 for an interactive graph
	float attenuation;

	// The field of view of the light, in degrees. This is the maximum angle from the light's forward vector,
	// so values of 180 or more will light everything
	float fov = 180.0f;

	// The direction the light is facing
	// As matricies are weird, this can have weird effects if the parent node is rotated
	Vector3 facing = Vector3(0, 1, 0);
};

