#pragma once

#include <memory>

class OGLRenderer;
class Texture;
class Shader;

struct Materiel {
	std::shared_ptr<Texture> diffuse;
	std::shared_ptr<Texture> normal;
	std::shared_ptr<Shader> shader;
	std::shared_ptr<Texture> noise = nullptr;

	void bind(OGLRenderer& r, const Materiel& defaults) const;
};
