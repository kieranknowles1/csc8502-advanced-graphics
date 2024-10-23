#pragma once

#include "../nclgl/OGLRenderer.h"
#include "../nclgl/Vector2.h"

class Renderer : public OGLRenderer
{
public:
	Renderer(Window& parent);
	~Renderer() override;
	
	void RenderScene() override;

	void UpdateTextureMatrix(float rotation, float scale, Vector2 position);
	void ToggleRepeating();
	void ToggleFiltering();

private:
	// Bind the active texture to a unit and set the uniform
	// unit should be one of the GL_TEXTURE enums
	void setTexture(GLenum unit, const char* uniformName, GLuint texture);

	Shader* shader;
	Mesh* triangle;
	GLuint texture;
	GLuint blendTexture;
	bool repeatEnabled;
	bool filterEnabled;
};
