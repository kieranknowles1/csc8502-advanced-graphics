#pragma once

#include "Stage.h"
#include "../Shader.h"

namespace PostProcess {
	class Blur : public Stage
	{
	public:
		Blur(int passes);
		~Blur();

		GLuint apply(
			OGLRenderer& r,
			GLuint sourceTexture,
			GLuint destinationFbo,
			GLuint scratchTextures[2]
		) override;

	private:
		int passes;
		Mesh* quad;
		Shader* shader;
	};
}
