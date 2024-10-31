#pragma once

#include <map>
#include <string>
#include <memory>

#include "glad/glad.h"

class Shader;
class Mesh;

// TODO: Use our own key type
using TextureKey = std::pair<std::string, int>;

// TODO: THis should be in its own file
class ManagedTexture
{
public:
	const static std::string CubeWestExt;
	const static std::string CubeEastExt;
	const static std::string CubeUpExt;
	const static std::string CubeDownExt;
	const static std::string CubeNorthExt;
	const static std::string CubeSouthExt;

	ManagedTexture(const std::string& name, GLenum type, GLuint id) : name(name), type(type), texture(id) {};

	ManagedTexture(const TextureKey& key);

	// TODO: These could probably be generics
	static std::shared_ptr<ManagedTexture> fromCubeMap(const std::string& name);

	~ManagedTexture();

	GLuint getId() const { return texture; }

	// Bind the texture to the active texture unit
	void bind();

private:
	std::string name;
	GLenum type;
	GLuint texture;
};


template <typename K, typename V>
class ResourceMap
{
public:
	std::shared_ptr<V> get(const K& key)
	{
		auto it = resources.find(key);
		if (it == resources.end() || it->second.expired())
		{
			auto resource = std::make_shared<V>(key);
			resources.emplace(key, resource);
			return resource;
		}

		return it->second.lock();
	}
private:
	std::map<K, std::weak_ptr<V>> resources;
};

class ResourceManager
{
public:
	ResourceManager();

	ResourceMap<TextureKey, ManagedTexture>& getTextures() { return textures; }

	std::shared_ptr<ManagedTexture> getCubeMap(const std::string& name);
protected:
	// TODO: Shaders need to be pairs of vertex and fragment shaders
	//std::map<std::string, Shader*> shaders;
	ResourceMap<TextureKey, ManagedTexture> textures;

	//std::map<std::pair<std::string, int>, std::weak_ptr<ManagedTexture>> textures;
	std::map<std::string, std::weak_ptr<ManagedTexture>> cubeMaps;
};
