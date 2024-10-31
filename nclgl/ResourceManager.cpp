#include "ResourceManager.h"

#include <stdexcept>
#include <filesystem>
#include <iostream>

#include "SOIL/SOIL.h"

#include "common.h"

const std::string ManagedTexture::CubeWestExt = "_west.jpg";
const std::string ManagedTexture::CubeEastExt = "_east.jpg";
const std::string ManagedTexture::CubeUpExt = "_up.jpg";
const std::string ManagedTexture::CubeDownExt = "_down.jpg";
const std::string ManagedTexture::CubeNorthExt = "_north.jpg";
const std::string ManagedTexture::CubeSouthExt = "_south.jpg";

ManagedTexture::ManagedTexture(const TextureKey& key)
{
	name = key.name;
	type = GL_TEXTURE_2D;

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

std::string ManagedTexture::describe() const
{
	return "Texture: " + name;
}

std::shared_ptr<ManagedTexture> ManagedTexture::fromCubeMap(const std::string& name)
{
	std::string base(std::string(TEXTUREDIR) + name);
	auto texture = SOIL_load_OGL_cubemap(
		(base + CubeWestExt).c_str(), (base + CubeEastExt).c_str(),
		(base + CubeUpExt).c_str(), (base + CubeDownExt).c_str(),
		(base + CubeSouthExt).c_str(), (base + CubeNorthExt).c_str(),
		SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0
	);

	if (texture == 0)
	{
		throw std::runtime_error("Failed to load cubemap: " + name);
	}

	return std::make_shared<ManagedTexture>(name, GL_TEXTURE_CUBE_MAP, texture);
}

ManagedTexture::~ManagedTexture()
{
	//std::cout << "Deleting texture " << name << " with id " << texture << std::endl;
	glDeleteTextures(1, &texture);
}

void ManagedTexture::bind()
{
	glBindTexture(type, texture);
}

ResourceManager::ResourceManager()
{
#ifdef _WIN32
	// FIXME: This is a hack to get the working directory to be the root of the project
	std::filesystem::current_path("H:/Documents/src/csc8502-advanced-graphics");
#endif // _WIN32


	auto pwd = std::filesystem::current_path().string();
	std::cout << "Using working directory: " << pwd << std::endl;
}

std::shared_ptr<ManagedTexture> ResourceManager::getCubeMap(const std::string& name)
{
	auto it = cubeMaps.find(name);
	if (it == cubeMaps.end() || it->second.expired())
	{
		auto tex = ManagedTexture::fromCubeMap(name);
		cubeMaps.emplace(name, tex);
		return tex;
	}
	return it->second.lock();
}
