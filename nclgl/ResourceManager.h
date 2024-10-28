#pragma once

#include <map>
#include <string>
#include <memory>

#include <glad/glad.h>

class Shader;
class Mesh;

class ManagedTexture
{
public:
	ManagedTexture(const std::string& name, unsigned int flags);
	~ManagedTexture();

	GLuint getId() const { return texture; }

	// Bind the texture to the active texture unit
	void bind();
private:
	GLuint texture;
};

class ResourceManager
{
public:
	// Get a texture by name, loading it if necessary
	std::shared_ptr<ManagedTexture> getTexture(const std::string& name, unsigned int flags);
protected:
	// TODO: Shaders need to be pairs of vertex and fragment shaders
	//std::map<std::string, Shader*> shaders;

	std::map<std::pair<std::string, int>, std::weak_ptr<ManagedTexture>> textures;
};
