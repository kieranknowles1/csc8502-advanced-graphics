#pragma once

#include "SceneNode.h"

class OGLRenderer;

class Light : public SceneNode
{
public:
	enum class Type {
		// A point light with an optional radius
		Point = 0,
		// A directional light that lights everything in a single direction with no falloff
		Sun = 1,
	};

	static const std::string PositionUniform;
	static const std::string ColorUniform;
	static const std::string RadiusUniform;
	static const std::string AttenuationUniform;
	static const std::string FOVUniform;
	static const std::string forwardUniform;
	static const std::string typeUniform;
	static const std::string shadowMatrixUniform;
	static const std::string projectedTextureUniform;
	static const std::string projectedTextureUseUniform;

	Light(float radius, float attenuation = 3.0f)
		: attenuation(attenuation) {
		setRadius(radius);
	}

	~Light(void) override {}

	// Bind this light's uniforms to the active shader
	void bind(const OGLRenderer& renderer) const;

	void setRadius(float r) {
		radius = r;
		boundingRadius = r;
	}
	float getRadius() const { return radius; }

	void setFov(float f) {
		if (f > 180.0f) f = 180.0f;
		if (f < 0.0f) f = 0.0f;
		fov = f;
	}
	float getFov() const { return fov; }

	void setFacing(const Vector3& f) { facing = f; }
	Vector3 getFacing() const { return facing; }

	void setType(Type t) { type = t; }
	Type getType() const { return type; }

	void setCastShadows(bool c) { castShadows = c; }
	bool getCastsShadows() const { return castShadows; }

	// The view matrix used for shadows and projected textures
	void setViewMatrix(const Matrix4& m) { viewMatrix = m; }
	const Matrix4& getViewMatrix() const { return viewMatrix; }

	// The projection matrix used for shadows and projected textures
	void setProjectionMatrix(const Matrix4& m) { projectionMatrix = m; }
	const Matrix4& getProjectionMatrix() const { return projectionMatrix; }

	void setProjectedTexture(std::shared_ptr<Texture> t) { projectedTexture = t; }
	std::shared_ptr<Texture> getProjectedTexture() const { return projectedTexture; }

	bool ignoreBounds() const override { return type == Type::Sun; }
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

	Type type = Type::Point;

	// Whether this light casts shadows
	// Expensive, so use sparingly
	bool castShadows = false;
	Matrix4 viewMatrix;
	Matrix4 projectionMatrix;

	std::shared_ptr<Texture> projectedTexture;

	const std::string getName() const override { return "Light"; }

	Light(const Light& l)
		: SceneNode(l)
		, radius(l.radius)
		, attenuation(l.attenuation)
		, fov(l.fov)
		, facing(l.facing)
		, type(l.type)
		, castShadows(l.castShadows)
		, viewMatrix(l.viewMatrix)
		, projectionMatrix(l.projectionMatrix)
		, projectedTexture(l.projectedTexture) {}
	SceneNode* deepCopy() const override {
		auto copy = new Light(*this);
		copy->copyChildrenFrom(*this);
		return new Light(*this);
	}
};

