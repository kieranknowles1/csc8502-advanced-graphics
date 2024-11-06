#pragma once

#include <memory>
#include <string>

class OGLRenderer;
class Texture;
class Shader;

struct Materiel {
	static Materiel fromFile(const std::string& filename);

	std::shared_ptr<Texture> diffuse;
	std::shared_ptr<Texture> normal;
	std::shared_ptr<Shader> shader;
	std::shared_ptr<Texture> noise = nullptr;

	void bind(OGLRenderer& r, const Materiel& defaults) const;
};
