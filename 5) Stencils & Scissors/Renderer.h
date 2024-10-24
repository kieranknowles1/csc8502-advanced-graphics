#pragma once

#include "../nclgl/OGLRenderer.h"
#include "../nclgl/Camera.h"

class Renderer : public OGLRenderer	
{
public:
	Renderer(Window &parent);
	~Renderer() override;

	void RenderScene() override;
	void UpdateScene(float dt) override;

	void ToggleScissor() { scissorActive = !scissorActive; }
	void ToggleStencil() { stencilActive = !stencilActive; }
	void ToggleMask() { showMask = !showMask; }

private:
	static const GLuint STAR_MASK = 0x02;

	Camera* camera;
	Shader* shader;
	Mesh* meshes[2];
	GLuint textures[2];

	bool scissorActive;
	bool stencilActive;
	bool showMask;
};

