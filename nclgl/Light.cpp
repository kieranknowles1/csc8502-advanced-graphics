#include "Light.h"

#include "OGLRenderer.h"
#include "Camera.h"

const std::string Light::PositionUniform = "lightPos";
const std::string Light::ColorUniform = "lightColor";
const std::string Light::RadiusUniform = "lightRadius";
const std::string Light::AttenuationUniform = "lightAttenuation";
const std::string Light::FOVUniform = "lightFOV";
const std::string Light::forwardUniform = "lightForward";
const std::string Light::typeUniform = "lightType";

void Light::bind(const OGLRenderer& renderer) const {
	auto shader = renderer.getCurrentShader();
	auto position = getWorldTransform().GetPositionVector();

	const Vector3* positionPtr;
	if (type == Type::Point) {
		positionPtr = &position;
	}
	else {
		positionPtr = &renderer.getCamera()->getPosition();
	}
	glUniform3f(
		shader->getUniform(PositionUniform.c_str()),
		positionPtr->x,
		positionPtr->y,
		positionPtr->z
	);
	glUniform3f(shader->getUniform(ColorUniform.c_str()), color.x, color.y, color.z);
	glUniform1f(shader->getUniform(RadiusUniform.c_str()), radius);
	glUniform1f(shader->getUniform(AttenuationUniform.c_str()), attenuation);

	Vector3 pointAhead = getWorldTransform() * (position + facing);
	Vector3 forward = (pointAhead - position).Normalised();

	glUniform1f(shader->getUniform(FOVUniform.c_str()), fov);
	glUniform3f(shader->getUniform(forwardUniform.c_str()), forward.x, forward.y, forward.z);

	glUniform1i(shader->getUniform(typeUniform.c_str()), (int)type);
}