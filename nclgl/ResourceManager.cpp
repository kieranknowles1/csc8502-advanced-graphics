#include "ResourceManager.h"

#include <stdexcept>

#include "SOIL/SOIL.h"

#include "common.h"

ManagedTexture::ManagedTexture(const std::string& name, unsigned int flags)
{
	auto fullPath = TEXTUREDIR + name;
	texture = SOIL_load_OGL_texture(
		fullPath.c_str(), SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, flags
	);

	if (texture == 0)
	{
		throw std::runtime_error("Failed to load texture: " + name);
	}
}

ManagedTexture::~ManagedTexture()
{
	glDeleteTextures(1, &texture);
}

void ManagedTexture::bind()
{
	glBindTexture(GL_TEXTURE_2D, texture);
}

std::shared_ptr<ManagedTexture> ResourceManager::getTexture(const std::string& name, unsigned int flags)
{
	auto key = std::make_pair(name, flags);
	auto it = textures.find(key);
	if (it == textures.end() || it->second.expired())
	{
		auto tex = std::make_shared<ManagedTexture>(name, flags);
		textures[key] = tex;
		return tex;
	}
	return it->second.lock();
}
