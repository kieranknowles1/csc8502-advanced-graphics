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

	Light(float radius, float attenuation = 2.0f)
		: radius(radius), attenuation(attenuation) {}

	~Light(void) {}

	// Bind this light's uniforms to the active shader
	void bind(const OGLRenderer& renderer) const;

protected:
	float radius;
	// How quickly the light fades with distance. 2.0 is a good default
	// Too small and there will be a sharp cut-off, too large and the light will be too weak
	// See https://www.desmos.com/calculator/ecqboyzch2 for an interactive graph
	float attenuation;
};

