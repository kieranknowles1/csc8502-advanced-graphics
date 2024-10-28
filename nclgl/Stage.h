#pragma once

#include <glad/glad.h>

#include "OGLRenderer.h"

namespace PostProcess {
	class Stage {
	public:
		// Apply the stage to the source texture,
		// writing the result to the destination FBO
		// using the scratch textures as temporary storage
		// Returns the texture ID of the result, which will be one
		// of the scratch textures.
		// TODO: Scratch space and FBOs should be their own `PostProcessContext` class
		virtual GLuint apply(
			OGLRenderer& r,
			GLuint sourceTexture,
			GLuint scratchFbo,
			// 2 textures for ping-pong
			// The stage can choose which order to use them in, so we return the one written to last
			// We also don't write to the source texture, as I'd rather be explicit about when it's OK to overwrite something
			GLuint scratchTextures[2]
		) = 0;
	};
}
