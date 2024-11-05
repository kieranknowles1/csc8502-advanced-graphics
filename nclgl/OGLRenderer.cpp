/*
Class:OGLRenderer
Author:Rich Davison	 <richard-gordon.davison@newcastle.ac.uk>
Description:Abstract base class for the graphics tutorials. Creates an OpenGL
3.2 CORE PROFILE rendering context. Each lesson will create a renderer that
inherits from this class - so all context creation is handled automatically,
but students still get to see HOW such a context is created.

-_-_-_-_-_-_-_,------,
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""

*/
#include "OGLRenderer.h"
#include "Shader.h"
#include <algorithm>

#include <SDL2/SDL.h>

#include "SceneNode.h"
#include "Light.h"

#include "Camera.h"

/*
Creates an OpenGL 3.2 CORE PROFILE rendering context. Sets itself
as the current renderer of the passed 'parent' Window. Not the best
way to do it - but it kept the Tutorial code down to a minimum!
*/
OGLRenderer::OGLRenderer(Window &window)
	: currentShader(nullptr)
	, width(window.GetScreenSize().x)
	, height(window.GetScreenSize().y)
	, rng(0) // Use the same seed for consistency
{
	glContext = SDL_GL_CreateContext(window.getSdlWindow());
	if (!glContext) {
		auto error = SDL_GetError();
		throw std::runtime_error("Failed to create OpenGL context: " + std::string(error));
	}

	if (!gladLoadGL()) {
		throw std::runtime_error("OGLRenderer::OGLRenderer(): Cannot initialise GLAD!");
	}

	char* ver = (char*)glGetString(GL_VERSION); // ver must equal "3.2.0" (or greater!)
	std::cout << "OGLRenderer::OGLRenderer(): Using OpenGL: " << ver << "\n";

	//If we get this far, everything's going well!

	glDebugMessageCallbackARB(&OGLRenderer::DebugCallback, NULL);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);

	glClearColor(0.2f,0.2f,0.2f,1.0f);			//When we clear the screen, we want it to be dark grey

	window.SetRenderer(this);					//Tell our window about the new renderer! (Which will in turn resize the renderer window to fit...)

	// Set up frame buffers for deferred rendering
	gBufferColor = generateScreenTexture();
	gBufferDepth = generateScreenTexture(/*depth=*/true);
	gBufferNormal = generateScreenTexture();

	deferredLightDiffuse = generateScreenTexture();
	deferredLightSpecular = generateScreenTexture();

	GLenum buffers[] = {
		GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1
	};

	glGenFramebuffers(1, &gBufferFbo);
	glGenFramebuffers(1, &deferredLightFbo);

	glBindFramebuffer(GL_FRAMEBUFFER, gBufferFbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gBufferColor, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gBufferNormal, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, gBufferDepth, 0);
	glDrawBuffers(2, buffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		throw std::runtime_error("Framebuffer not complete");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, deferredLightFbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, deferredLightDiffuse, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, deferredLightSpecular, 0);
	glDrawBuffers(2, buffers);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		throw std::runtime_error("Framebuffer not complete");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	quad.reset(Mesh::GenerateQuad());
}

/*
Destructor. Deletes the default shader, and the OpenGL rendering context.
*/
OGLRenderer::~OGLRenderer(void)	{
	SDL_GL_DeleteContext(glContext);

	glDeleteFramebuffers(1, &gBufferFbo);
	glDeleteFramebuffers(1, &deferredLightFbo);

	glDeleteTextures(1, &gBufferColor);
	glDeleteTextures(1, &gBufferDepth);
	glDeleteTextures(1, &gBufferNormal);
	glDeleteTextures(1, &deferredLightDiffuse);
	glDeleteTextures(1, &deferredLightSpecular);
}

GLuint OGLRenderer::generateScreenTexture(bool depth) {
	GLuint tex;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	auto format = depth ? GL_DEPTH_COMPONENT24 : GL_RGBA8;
	auto type = depth ? GL_DEPTH_COMPONENT : GL_RGBA;

	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, type, GL_UNSIGNED_BYTE, nullptr);
	glBindTexture(GL_TEXTURE_2D, 0);

	return tex;
}

/*
Resizes the rendering area. Should only be called by the Window class!
Does lower bounds checking on input values, so should be reasonably safe
to call.
*/
void OGLRenderer::Resize(int x, int y)	{
	width	= std::max(x,1);
	height	= std::max(y,1);
	glViewport(0,0,width,height);
}

/*
Used by some later tutorials when we want to have framerate-independent
updates on certain datatypes. Really, OGLRenderer should have its own
timer, so it can just sit and look after itself (potentially even in
another thread...), but it's fine for the tutorials.

STUDENTS: Don't put your entity update routine in this, or anything like
that - it's just asking for trouble! Strictly speaking, even the camera
shouldn't be in here...(I'm lazy)
*/
void OGLRenderer::UpdateScene(float msec)	{

}

/*
Updates the uniform matrices of the current shader. Assumes that
the shader has uniform matrices called modelMatrix, viewMatrix,
projMatrix, and textureMatrix. Updates them with the relevant
matrix data. Sanity checks currentShader, so is always safe to
call.
*/
void OGLRenderer::UpdateShaderMatrices()	{
	if(currentShader) {
		glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix")   ,	1,false, modelMatrix.values);
		glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "viewMatrix")    ,	1,false, viewMatrix.values);
		glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "projMatrix")    ,	1,false, projMatrix.values);
		glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "textureMatrix") , 1,false, textureMatrix.values);
		glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "shadowMatrix")  , 1,false, shadowMatrix.values);
	}
}

void OGLRenderer::BindShader(Shader* s) {
	currentShader = s;
	glUseProgram(s ? s->GetProgram() : 0);
}

void OGLRenderer::DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)	{
		std::string sourceName;
		std::string typeName;
		std::string severityName;

		if (type == GL_DEBUG_TYPE_OTHER_ARB) {
			return;
		}

		switch(source) {
			case GL_DEBUG_SOURCE_API_ARB			: sourceName = "Source(OpenGL)"			;break;
			case GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB	: sourceName = "Source(Window System)"	;break;
			case GL_DEBUG_SOURCE_SHADER_COMPILER_ARB: sourceName = "Source(Shader Compiler)";break;
			case GL_DEBUG_SOURCE_THIRD_PARTY_ARB	: sourceName = "Source(Third Party)"	;break;
			case GL_DEBUG_SOURCE_APPLICATION_ARB	: sourceName = "Source(Application)"	;break;
			case GL_DEBUG_SOURCE_OTHER_ARB			: sourceName = "Source(Other)"			;break;
		}

		switch(type) {
			case GL_DEBUG_TYPE_ERROR_ARB				: typeName = "Type(Error)"					;break;
			case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB	: typeName = "Type(Deprecated Behaviour)"	;break;
			case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB	: typeName = "Type(Undefined Behaviour)"	;break;
			case GL_DEBUG_TYPE_PORTABILITY_ARB			: typeName = "Type(Portability)"			;break;
			case GL_DEBUG_TYPE_PERFORMANCE_ARB			: typeName = "Type(Performance)"			;break;
			case GL_DEBUG_TYPE_OTHER_ARB				: typeName = "Type(Other)"					;break;
			default: {
				return;
			}
		}

		switch(severity) {
			case GL_DEBUG_SEVERITY_HIGH_ARB		: severityName = "Priority(High)"		;break;
			case GL_DEBUG_SEVERITY_MEDIUM_ARB	: severityName = "Priority(Medium)"		;break;
			case GL_DEBUG_SEVERITY_LOW_ARB		: severityName = "Priority(Low)"		;break;
		}

		std::cout << "OpenGL Debug Output: " + sourceName + ", " + typeName + ", " + severityName + ", " + std::string(message) << "\n";
}

void OGLRenderer::drawTree(SceneNode* root) {
	clearNodeLists();

	buildNodeLists(root);
	sortNodeLists();

	// Pass 1 - Draw scene into g-buffer
	glBindFramebuffer(GL_FRAMEBUFFER, gBufferFbo);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	drawNodes(opaqueNodes);
	drawNodes(transparentNodes);

	// Pass 2 - Draw point lights
	drawPointLights();

	// Pass 3 - Combine
	// TODO: Also do post-processing here
	combineBuffers();
}

void OGLRenderer::drawPointLights() {
	glBindFramebuffer(GL_FRAMEBUFFER, deferredLightFbo);
	BindShader(pointLightShader.get());

	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	// Combine lights additively
	glBlendFunc(GL_ONE, GL_ONE);
	// Draw each sphere exactly once, even if the camera is inside it
	glCullFace(GL_FRONT);
	// Don't occlude anything
	glDepthFunc(GL_ALWAYS);
	glDepthMask(GL_FALSE);

	// Pass in the g-buffer textures
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gBufferDepth);
	glUniform1i(pointLightShader->getUniform("depthTex"), 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gBufferNormal);
	glUniform1i(pointLightShader->getUniform("normalTex"), 1);

	glUniform3f(
		pointLightShader->getUniform("cameraPos"),
		camera->getPosition().x, camera->getPosition().y, camera->getPosition().z
	);

	// Use to calculate coordinates for dp	
	glUniform2f(pointLightShader->getUniform("pixelSize"),
		1.0f / width, 1.0f / height
	);

	// Save having to calculate this in the shader
	Matrix4 invViewProj = (projMatrix * viewMatrix).Inverse();
	invViewProj.bind(pointLightShader->getUniform("inverseProjView"));

	UpdateShaderMatrices();
	for (auto& light : lights) {
		light->bind(*this);
		sphere->Draw();
	}

	// Restore the state
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glCullFace(GL_BACK);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OGLRenderer::combineBuffers() {
	BindShader(combineShader.get());
	auto oldModel = modelMatrix;
	auto oldView = viewMatrix;
	auto oldProj = projMatrix;
	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();
	UpdateShaderMatrices();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gBufferColor);
	glUniform1i(combineShader->getUniform("diffuseTex"), 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, deferredLightDiffuse);
	glUniform1i(combineShader->getUniform("diffuseLight"), 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, deferredLightSpecular);
	glUniform1i(combineShader->getUniform("specularLight"), 2);

	quad->Draw();

	modelMatrix = oldModel;
	viewMatrix = oldView;
	projMatrix = oldProj;
}

void OGLRenderer::buildNodeLists(SceneNode* from) {
	if (!from->isEnabled()) {
		return;
	}

	if (from->getMesh()) {
		auto& list = from->getIsTransparent() ? transparentNodes : opaqueNodes;
		list.push_back(from);

	}

	auto asLight = dynamic_cast<Light*>(from);
	if (asLight) {
		lights.push_back(asLight);
	}

	for (auto child = from->childrenBegin(); child != from->childrenEnd(); child++) {
		buildNodeLists(*child);
	}
}

void OGLRenderer::sortNodeLists() {
}

void OGLRenderer::clearNodeLists() {
	opaqueNodes.clear();
	transparentNodes.clear();
	lights.clear();
}

void OGLRenderer::drawNodes(const std::vector<SceneNode*>& nodes) {
	for (auto& node : nodes) {
		node->drawSelf(*this);
	}
}
