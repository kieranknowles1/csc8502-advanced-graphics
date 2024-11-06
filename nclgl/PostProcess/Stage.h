#pragma once

#include "glad/glad.h"

#include "../OGLRenderer.h"

namespace PostProcess {
	class Stage {
	public:
		virtual ~Stage() {}

		// Apply the stage, outputting to the active framebuffer
		// Inputs for the specific stage should be passed in directly
		virtual void apply(OGLRenderer& r) = 0;

		//virtual GLuint apply(
		//	OGLRenderer& r,
		//	GLuint sourceTexture,
		//	GLuint scratchFbo,
		//	// 2 textures for ping-pong
		//	// The stage can choose which order to use them in, so we return the one written to last
		//	// We also don't write to the source texture, as I'd rather be explicit about when it's OK to overwrite something
		//	GLuint scratchTextures[2]
		//) = 0;
	};
}
