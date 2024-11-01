#pragma once

#include "ResourceManager.h"

struct Materiel {
	std::shared_ptr<Texture> diffuse;
	std::shared_ptr<Texture> normal;
	std::shared_ptr<Shader> shader;
};