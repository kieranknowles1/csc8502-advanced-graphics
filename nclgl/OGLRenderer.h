#pragma once
/*
Class:OGLRenderer
Author:Rich Davison	 <richard-gordon.davison@newcastle.ac.uk>
Description:Abstract base class for the graphics tutorials. Creates an OpenGL
3.2+ CORE PROFILE rendering context. Each lesson will create a renderer that
inherits from this class - so all context creation is handled automatically,
but students still get to see HOW such a context is created.

-_-_-_-_-_-_-_,------,
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""

*/
#include "common.h"

#include <string>
#include <fstream>
#include <vector>

#include <GL/glew.h>

#include "GL/gl.h"

#include "SOIL/SOIL.h"

#include "Vector4.h"
#include "Vector3.h"
#include "Vector2.h"
#include "Quaternion.h"
#include "Matrix4.h"
#include "Window.h"
#include "Shader.h"
#include "Mesh.h"

using std::vector;

using SDL_GLContext = void*;

class Shader;
class Light;
class Mesh;
class Window;

struct DebugSettings {
	// Whether to draw the bounding boxes of nodes
	bool drawBoundingBoxes = false;
};

class OGLRenderer	{
public:
	friend class Window;
	OGLRenderer(Window &parent);
	virtual ~OGLRenderer(void);

	virtual void	RenderScene()		= 0;
	virtual void	UpdateScene(float msec);

	void setTextureRepeating(GLuint texture, bool repeating);

	Shader* getCurrentShader() const { return currentShader; }

	void			BindShader(Shader*s);
	void			UpdateShaderMatrices();

	void setDebugSettings(const DebugSettings& settings) {
		debugSettings = settings;
	}

	const DebugSettings& getDebugSettings() const {
		return debugSettings;
	}

	const Mesh* getDebugCube() const {
		return debugCube;
	}
protected:
	virtual void	Resize(int x, int y);

	void StartDebugGroup(const std::string& s) {
		glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, (GLsizei)s.length(), s.c_str());
	}

	void EndDebugGroup() {
		glPopDebugGroup();
	}

	Matrix4 projMatrix;		//Projection matrix
	Matrix4 modelMatrix;	//Model matrix. NOT MODELVIEW
	Matrix4 viewMatrix;		//View matrix
	Matrix4 textureMatrix;	//Texture matrix
	Matrix4 shadowMatrix;

	int		width;			//Render area width (not quite the same as window width)
	int		height;			//Render area height (not quite the same as window height)

	DebugSettings debugSettings;
	Mesh* debugCube;

private:
	Shader* currentShader;
	SDL_GLContext glContext;

	static void DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
};
