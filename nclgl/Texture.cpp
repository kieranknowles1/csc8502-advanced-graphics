#include "Texture.h"

#include <stdexcept>

#include <SOIL/SOIL.h>

#include "common.h"

const std::string Texture::CubeWestExt = "_west.jpg";
const std::string Texture::CubeEastExt = "_east.jpg";
const std::string Texture::CubeUpExt = "_up.jpg";
const std::string Texture::CubeDownExt = "_down.jpg";
const std::string Texture::CubeNorthExt = "_north.jpg";
const std::string Texture::CubeSouthExt = "_south.jpg";

Texture::Texture(const TextureKey& key)
    : name(key.name), type(GL_TEXTURE_2D)
{
	auto fullPath = TEXTUREDIR + key.name;
	texture = SOIL_load_OGL_texture(
		fullPath.c_str(), SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, key.soilFlags
	);
	if (texture == 0) {
		throw std::runtime_error("Failed to load texture: " + name);
	}

	auto repeat = key.repeat ? GL_REPEAT : GL_CLAMP;
	glBindTexture(type, texture);
	glTexParameteri(type, GL_TEXTURE_WRAP_S, repeat);
	glTexParameteri(type, GL_TEXTURE_WRAP_T, repeat);
}

Texture::Texture(const std::string& name)
    : name(name), type(GL_TEXTURE_CUBE_MAP)
{
	std::string base(std::string(TEXTUREDIR) + name);
	texture = SOIL_load_OGL_cubemap(
		(base + CubeWestExt).c_str(), (base + CubeEastExt).c_str(),
		(base + CubeUpExt).c_str(), (base + CubeDownExt).c_str(),
		(base + CubeSouthExt).c_str(), (base + CubeNorthExt).c_str(),
		SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0
	);

	if (texture == 0)
	{
		throw std::runtime_error("Failed to load cubemap: " + name);
	}
}

Texture::~Texture()
{
    glDeleteTextures(1, &texture);
}

void Texture::bind()
{
    glBindTexture(type, texture);
}

std::string Texture::describe() const
{
    std::string typeStr = type == GL_TEXTURE_2D ? "Texture" : "CubeMap";
    return typeStr + ": " + name;
}
