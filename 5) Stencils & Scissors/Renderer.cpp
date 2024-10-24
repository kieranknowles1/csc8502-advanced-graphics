#include "Renderer.h"

Renderer::Renderer(Window &parent)
	: OGLRenderer(parent)
	, scissorActive(false)
	, stencilActive(false)
	, showMask(false)
	, camera(new Camera())
	, shader(new Shader("TexturedVertex.glsl", "StencilFragment.glsl"))
{
	meshes[0] = Mesh::GenerateTriangle();
	meshes[1] = Mesh::GenerateQuad();

	textures[0] = SOIL_load_OGL_texture(
		TEXTUREDIR"brick.tga",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		0
	);

	textures[1] = SOIL_load_OGL_texture(
		TEXTUREDIR"star.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		0
	);

	setTextureRepeating(textures[0], true);
	setTextureRepeating(textures[1], true);

	init = shader->LoadSuccess() && textures[0] && textures[1];
}

Renderer::~Renderer()
{
	delete camera;
	delete shader;
	delete meshes[0];
	delete meshes[1];
	glDeleteTextures(2, textures);
}

void Renderer::RenderScene()
{
	glClear(
		GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT
	);

	if (scissorActive)
	{
		glEnable(GL_SCISSOR_TEST);
		glScissor(
			(float)width / 2.5, (float)height / 2.5,
			(float)width / 5, (float)height / 5
		);
	}

	BindShader(shader);
	textureMatrix = Matrix4::Scale(Vector3(6, 6, 6));
	UpdateShaderMatrices();

	if (stencilActive) {
		glEnable(GL_STENCIL_TEST);
		// Disable writing to the color buffer, we only
		// want to write to the stencil buffer
		if (!showMask)
			glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

		glStencilFunc(GL_ALWAYS, STAR_MASK, 0xFF);
		// Write STAR_MASK for all fragments that aren't discarded
		glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
		glBindTexture(GL_TEXTURE_2D, textures[1]);
		meshes[1]->Draw();

		// Re-enable writing to the color buffer
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		// Mask out places where STAR_MASK is set
		glStencilFunc(GL_NOTEQUAL, STAR_MASK, 0xFF);
		// Disable writing to the stencil buffer
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	}

	glUniform1i(glGetUniformLocation(shader->GetProgram(), "diffuseTex"), 0);

	glBindTexture(GL_TEXTURE_2D, textures[0]);
	meshes[0]->Draw();

	// Scissors and stencil tests apply to glClear, so turn
	// them off once we're done
	glDisable(GL_SCISSOR_TEST);
	glDisable(GL_STENCIL_TEST);
}

void Renderer::UpdateScene(float dt)
{
	//camera->update(dt);
	//viewMatrix = camera->buildViewMatrix();
}
