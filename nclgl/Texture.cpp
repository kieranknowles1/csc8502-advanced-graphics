#include "Texture.h"

#include <stdexcept>

#include <SOIL/SOIL.h>

#include "Shader.h"
#include "common.h"

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
		(base + "_px.png").c_str(), (base + "_nx.png").c_str(),
		(base + "_py.png").c_str(), (base + "_ny.png").c_str(),
		(base + "_pz.png").c_str(), (base + "_nz.png").c_str(),
		SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0
	);
	// This is needed to avoid a seam at texture edges
	glTexParameteri(type, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(type, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	if (texture == 0)
	{
		throw std::runtime_error("Failed to load cubemap: " + name);
	}
}

Texture::~Texture()
{
    glDeleteTextures(1, &texture);
}

void Texture::bind() const
{
    glBindTexture(type, texture);
}

std::string Texture::describe() const
{
    std::string typeStr = type == GL_TEXTURE_2D ? "Texture" : "CubeMap";
    return typeStr + ": " + name;
}

void TextureUnit::Unit::bind(const Shader& shader, const Texture& texture) const
{
	glActiveTexture(enumValue());
	texture.bind();
	glUniform1i(shader.getUniform(uniform), intValue());
}
