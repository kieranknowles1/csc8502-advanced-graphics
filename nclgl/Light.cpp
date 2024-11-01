#include "Light.h"

#include "OGLRenderer.h"

const std::string Light::PositionUniform = "lightPos";
const std::string Light::ColorUniform = "lightColor";
const std::string Light::RadiusUniform = "lightRadius";
const std::string Light::AttenuationUniform = "lightAttenuation";
const std::string Light::FOVUniform = "lightFOV";
const std::string Light::forwardUniform = "lightForward";

void Light::bind(const OGLRenderer& renderer) const {
	auto shader = renderer.getCurrentShader();
	auto position = getWorldTransform().GetPositionVector();
	glUniform3fv(shader->getUniform(PositionUniform.c_str()), 1, (float*)&position);
	glUniform3f(shader->getUniform(ColorUniform.c_str()), color.x, color.y, color.z);
	glUniform1f(shader->getUniform(RadiusUniform.c_str()), radius);
	glUniform1f(shader->getUniform(AttenuationUniform.c_str()), attenuation);

	Vector3 pointAhead = getWorldTransform() * (position + facing);
	Vector3 forward = (pointAhead - position).Normalised();

	glUniform1f(shader->getUniform(FOVUniform.c_str()), fov);
	glUniform3f(shader->getUniform(forwardUniform.c_str()), forward.x, forward.y, forward.z);
}