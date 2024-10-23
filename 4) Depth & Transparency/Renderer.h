#pragma once

#include "../nclgl/OGLRenderer.h"

class Renderer : public OGLRenderer
{
public:
	Renderer(Window& parent);
	~Renderer() override;

	void RenderScene() override;

	void ToggleObject();
	void ToggleDepth();
	void ToggleAlphaBlend();
	void ToggleBlendMode();
	void MoveObject(float by);

	void SetAlphaThreshold(float value) { alphaThreshold = value; }

protected:
	GLuint textures[2];
	Mesh* meshes[2];
	Shader* shader;
	Vector3 positions[2];

	int activeObject;
	bool useDepth;
	bool useAlpha;
	int blendMode;
	float alphaThreshold;
};
