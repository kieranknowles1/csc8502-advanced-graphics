#include "Blur.h"

#include <stdexcept>

namespace PostProcess {
	Blur::Blur(int passes)
		: passes(passes)
		, quad(Mesh::GenerateQuad())
	{
		shader = new Shader(
			"post/passthroughVertex.glsl",
			"post/blur.glsl"
		);
		if (!shader->LoadSuccess()) throw std::runtime_error("Failed to load blur shader");
	}

	Blur::~Blur()
	{
		delete shader;
		delete quad;
	}

	GLuint Blur::apply(OGLRenderer& r, GLuint sourceTexture, GLuint scratchFbo, GLuint scratchTextures[2])
	{
		glBindFramebuffer(GL_FRAMEBUFFER, scratchFbo);
		r.BindShader(shader);

		glActiveTexture(GL_TEXTURE0);
		glUniform1i(shader->getUniform("sceneTex"), 0);

		for (int i = 0; i < passes; i++) {
			// Read from tex[0] or source, write to tex[1]
			glFramebufferTexture2D(
				GL_FRAMEBUFFER,
				GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
				scratchTextures[1], 0
			);
			glUniform1i(shader->getUniform("isVertical"), 0);
			glBindTexture(GL_TEXTURE_2D, i == 0 ? sourceTexture : scratchTextures[0]);
			quad->Draw();

			// Do it again with the colour buffer swapped
			glFramebufferTexture2D(
				GL_FRAMEBUFFER,
				GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
				scratchTextures[0], 0
			);
			glUniform1i(shader->getUniform("isVertical"), 1);
			glBindTexture(GL_TEXTURE_2D, scratchTextures[1]);
			quad->Draw();
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		return scratchTextures[0];
	}
}
