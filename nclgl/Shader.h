/******************************************************************************
Class:Shader
Implements:
Author:Rich Davison	 <richard-gordon.davison@newcastle.ac.uk>
Description:VERY simple class to encapsulate GLSL shader loading, linking,
and binding. Useful additions to this class would be overloaded functions to
replace the glUniformxx functions in external code, and possibly a map to store
uniform names and their resulting bindings.

-_-_-_-_-_-_-_,------,
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""

*//////////////////////////////////////////////////////////////////////////////

#pragma once
#include <set>

#include "OGLRenderer.h"

enum ShaderStage {
	SHADER_VERTEX,
	SHADER_FRAGMENT,
	SHADER_GEOMETRY,
	SHADER_DOMAIN,
	SHADER_HULL,
	SHADER_MAX
};

struct ShaderKey {
	std::string vertex;
	std::string fragment;
	std::string geometry;
	std::string domain;
	std::string hull;

	ShaderKey(const std::string& vertex, const std::string& fragment, const std::string& geometry = "", const std::string& domain = "", const std::string& hull = "")
		: vertex(vertex), fragment(fragment), geometry(geometry), domain(domain), hull(hull) {}

	bool operator<(const ShaderKey& other) const {
		if (vertex != other.vertex) return vertex < other.vertex;
		if (fragment != other.fragment) return fragment < other.fragment;
		if (geometry != other.geometry) return geometry < other.geometry;
		if (domain != other.domain) return domain < other.domain;
		return hull < other.hull;
	}
};

class Shader	{
public:
	Shader(const std::string& vertex, const std::string& fragment, const std::string& geometry = "", const std::string& domain = "", const std::string& hull = "");
	Shader(const ShaderKey& key)
		: Shader(key.vertex, key.fragment, key.geometry, key.domain, key.hull) {}
	~Shader(void);

	std::string describe() const;

	GLuint  GetProgram() { return programID;}

	void	Reload(bool deleteOld = true);

	bool LoadSuccess() {
		if (programValid != GL_TRUE) {
			return false;
		}
		for (int i = 0; i < SHADER_MAX; ++i) {
			if (shaderValid[i] != GL_TRUE) {
				return false;
			}
		}
		return true;
	}

	static void ReloadAllShaders();
	static void	PrintCompileLog(GLuint object);
	static void	PrintLinkLog(GLuint program);

	int getUniform(const char* name) const {
		int id = glGetUniformLocation(programID, name);
		if (id < 0) {
			// Only warn once per uniform
			if (badUniforms.find(name) == badUniforms.end()) {
				// This function isn't really const, but this is a debug
				// path anyway, so I don't consider it a big deal
				auto notConstThis = const_cast<Shader*>(this);
				notConstThis->badUniforms.insert(name);
				std::cerr << "Warning: " << name << " not found in shader " << describe() << std::endl;
			}
		}

		return id;
	}

protected:
	// Check if we have any incorrect names that could cause tricky bugs
	void checkBannedNames();
	void checkBannedName(const std::string& bad, const std::string& alt);

	void	DeleteIDs();

	bool	LoadShaderFile(const  std::string& from, std::string &into);
	void	GenerateShaderObject(unsigned int i);
	void	SetDefaultAttributes();
	void	LinkProgram();

	GLuint	programID;
	GLuint	objectIDs[SHADER_MAX];
	GLint	programValid;
	GLint	shaderValid[SHADER_MAX];

	std::string  shaderFiles[SHADER_MAX];

	std::set<std::string> badUniforms;

	static std::vector<Shader*> allShaders;
};

