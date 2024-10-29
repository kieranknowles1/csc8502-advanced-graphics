#pragma once

#include "Vector3.h"
#include "Vector4.h"

class OGLRenderer;

class Light
{
public:
	static const std::string PositionUniform;
	static const std::string ColorUniform;
	static const std::string RadiusUniform;

	Light(const Vector3& position, const Vector4& color, float radius)
		: position(position), color(color), radius(radius) {}

	~Light(void) {}

	// Bind this light's uniforms to the active shader
	void bind(const OGLRenderer& renderer) const;

protected:
	Vector3 position;
	Vector4 color;
	float radius;
};

