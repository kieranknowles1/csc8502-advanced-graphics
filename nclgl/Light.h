#pragma once

#include "Vector3.h"

class OGLRenderer;

class Light
{
public:
	static const std::string PositionUniform;
	static const std::string ColorUniform;
	static const std::string RadiusUniform;
	static const std::string AttenuationUniform;

	Light(const Vector3& position, const Vector3& color, float radius, float attenuation = 2.0f)
		: position(position), color(color), radius(radius), attenuation(attenuation) {}

	~Light(void) {}

	// Bind this light's uniforms to the active shader
	void bind(const OGLRenderer& renderer) const;

protected:
	Vector3 position;
	Vector3 color;
	float radius;
	// How quickly the light fades with distance. 2.0 is a good default
	// Too small and there will be a sharp cut-off, too large and the light will be too weak
	// See https://www.desmos.com/calculator/ecqboyzch2 for an interactive graph
	float attenuation;
};

