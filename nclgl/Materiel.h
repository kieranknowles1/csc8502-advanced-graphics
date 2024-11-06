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
	static std::vector<Materiel> fromFile(ResourceManager* rm, const std::string& filename);
	const static GLenum DefaultCullMode = GL_BACK;

	std::shared_ptr<Texture> diffuse;
	std::shared_ptr<Texture> normal;
	std::shared_ptr<Shader> shader;
	std::shared_ptr<Texture> noise = nullptr;
	GLenum cullMode = DefaultCullMode;

	void bind(OGLRenderer& r, const Materiel& defaults) const;
};
