#pragma once

#include <map>
#include <string>
#include <memory>

#include "glad/glad.h"

class Shader;
class Mesh;

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

	// TODO: These could probably be generics
	static std::shared_ptr<ManagedTexture> fromFile(const std::string& name, unsigned int flags);
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

class ResourceManager
{
public:
	ResourceManager();

	// Get a texture by name, loading it if necessary
	std::shared_ptr<ManagedTexture> getTexture(const std::string& name, unsigned int flags);

	std::shared_ptr<ManagedTexture> getCubeMap(const std::string& name);
protected:
	// TODO: Shaders need to be pairs of vertex and fragment shaders
	//std::map<std::string, Shader*> shaders;

	std::map<std::pair<std::string, int>, std::weak_ptr<ManagedTexture>> textures;
	std::map<std::string, std::weak_ptr<ManagedTexture>> cubeMaps;
};
