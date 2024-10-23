#pragma once

#include "../nclgl/OGLRenderer.h"

class Renderer : public OGLRenderer
{
public:
	Renderer(Window& parent);
	~Renderer() override;
	
	void RenderScene() override;

	void UpdateTextureMatrix(float rotation, float scale);
	void ToggleRepeating();
	void ToggleFiltering();

private:
	Shader* shader;
	Mesh* triangle;
	GLuint texture;
	bool repeatEnabled;
	bool filterEnabled;
};
