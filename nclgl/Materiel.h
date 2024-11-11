#pragma once

#include <glad/glad.h>

#include <memory>
#include <string>
#include <vector>

class OGLRenderer;
class Texture;
class Shader;
class ResourceManager;

struct Materiel {
	// Load from a MeshMaterial file
	// The following keys are supported:
	//  - Diffuse: The path to the diffuse texture
	//  - Bump: The path to the normal map
	//  - Cull: The cull mode to use. Can be GL_FRONT, GL_BACK, or GL_NONE, default is GL_BACK
	//  - InvertY: If true, the texture will be flipped vertically, default is false
	static std::vector<Materiel> fromFile(ResourceManager* rm, const std::string& filename);
	const static GLenum DefaultCullMode = GL_BACK;

	std::shared_ptr<Texture> diffuse;
	std::shared_ptr<Texture> normal;
	std::shared_ptr<Shader> shader;
	std::shared_ptr<Texture> noise = nullptr;
	GLenum cullMode = DefaultCullMode;

	void bind(OGLRenderer& r, const Materiel& defaults) const;
};
