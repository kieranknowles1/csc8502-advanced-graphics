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
#include <memory>
#include <random>

#include <glad/glad.h>

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
#include "Frustum.h"
#include "Materiel.h"

using std::vector;

using SDL_GLContext = void*;

class Shader;
class Light;
class Mesh;
class Window;
class SceneNode;
class Camera;
class ResourceManager;

struct DebugSettings {
	// Whether to draw the bounding boxes of nodes
	bool drawBoundingBoxes = false;
};

// Scratchpad for rendering a scene
// Kept between frames to reduce memory allocation
struct RenderContext {
	std::vector<Light*> pointLights;
	std::vector<Light*> shadowLights;
	std::vector<SceneNode*> transparentNodes;
	std::vector<SceneNode*> opaqueNodes;

	void clear();
};

class OGLRenderer	{
public:
	const static int StencilMaskObject = 0x01;

	friend class Window;
	OGLRenderer(Window &parent);
	virtual ~OGLRenderer(void);

	virtual void	RenderScene()		= 0;
	virtual void	UpdateScene(float msec);

	Shader* getCurrentShader() const { return currentShader; }
	Camera* getCamera() const { return camera.get(); }

	void			BindShader(Shader*s);
	void			UpdateShaderMatrices();

	void setDebugSettings(const DebugSettings& settings) {
		debugSettings = settings;
	}

	const DebugSettings& getDebugSettings() const {
		return debugSettings;
	}

	const Materiel& getDefaultMateriel() const {
		return defaultMateriel;
	}

	void setLockFrustum(bool lock) {
		lockFrustum = lock;
	}
	bool isFrustumLocked() const {
		return lockFrustum;
	}

protected:
	virtual void	Resize(int x, int y);

	// Put this early in the declaration so it's destroyed after
	// its users
	std::unique_ptr<ResourceManager> resourceManager;

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

	// Render a tree to a FBO or the screen
	// If a skybox is present, the FBO's stencil buffer will be replaced with a mask
	void drawTree(SceneNode* root, GLuint destFbo = 0);

	void setDefaultMateriel(const Materiel& materiel) {
		defaultMateriel = materiel;
	}
	Materiel defaultMateriel;

	std::mt19937 rng;

	std::unique_ptr<Camera> camera;
	bool lockFrustum = false;
	Frustum viewFrustum;

	// Used to draw deferred lighting
	// Directional lights are implemented with fragment discards
	std::shared_ptr<Mesh> sphere;

	void setPointLightShader(std::shared_ptr<Shader> shader) {
		pointLightShader = shader;
	}
	void setCombineShader(std::shared_ptr<Shader> shader) {
		combineShader = shader;
	}
	std::shared_ptr<Texture> skyTexture;
	std::shared_ptr<Shader> skyShader;

	GLuint generateScreenTexture(bool depth = false, GLenum clampMode = GL_CLAMP_TO_EDGE);

	void drawNodes(const std::vector<SceneNode*>& nodes, bool shadowPass = false);
	void buildNodeLists(SceneNode* from);

	// TODO: Frustum culling
	RenderContext context;
// private:
	void sortNodeLists();

	void drawSky(GLuint destFbo);

	GLuint gBufferFbo;
	GLuint gBufferDepth;
	GLuint gBufferColor;
	GLuint gBufferNormal;

	std::shared_ptr<Shader> pointLightShader;
	GLuint deferredLightFbo;
	GLuint deferredLightDiffuse;
	GLuint deferredLightSpecular;
	virtual void drawShadowLights() = 0; // TODO: Implement
	void drawPointLights();

	void beginLightPass();
	void endLightPass();

	std::shared_ptr<Shader> combineShader;
	std::unique_ptr<Mesh> quad;
	void combineBuffers(GLuint destFbo = 0);

	Shader* currentShader;
	SDL_GLContext glContext;

	static void DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
};
