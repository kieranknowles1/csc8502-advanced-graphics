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

using std::string;



static const float biasValues[16] = {
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 0.5, 0.0,
	0.5, 0.5, 0.5, 1.0
};
static const Matrix4 biasMatrix(biasValues);

/*
Creates an OpenGL 3.2 CORE PROFILE rendering context. Sets itself
as the current renderer of the passed 'parent' Window. Not the best
way to do it - but it kept the Tutorial code down to a minimum!
*/
OGLRenderer::OGLRenderer(Window &window)	{
	init					= false;
	HWND windowHandle = window.GetHandle();

	// Did We Get A Device Context?
	if (!(deviceContext=GetDC(windowHandle)))		{					
		std::cout << "OGLRenderer::OGLRenderer(): Failed to create window!\n";
		return;
	}

	glContext = SDL_GL_CreateContext(window.getSdlWindow());
	if (!glContext) {
		auto error = SDL_GetError();
		throw std::runtime_error("Failed to create OpenGL context: " + std::string(error));
	}

	if (!gladLoadGL()) {
		std::cout << "OGLRenderer::OGLRenderer(): Cannot initialise GLAD!\n";	//It's all gone wrong!
		return;
	}

	//Now we have a temporary context, we can find out if we support OGL 3.x
	char* ver = (char*)glGetString(GL_VERSION); // ver must equal "3.2.0" (or greater!)

	std::cout << "OGLRenderer::OGLRenderer(): Using OpenGL: " << ver << "\n";

	//If we get this far, everything's going well!

#ifdef OPENGL_DEBUGGING
	glDebugMessageCallbackARB(&OGLRenderer::DebugCallback, NULL);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
#endif

	glClearColor(0.2f,0.2f,0.2f,1.0f);			//When we clear the screen, we want it to be dark grey

	currentShader = 0;							//0 is the 'null' object name for shader programs...

	window.SetRenderer(this);					//Tell our window about the new renderer! (Which will in turn resize the renderer window to fit...)

	debugCube = Mesh::LoadDebugCube(); // Generic cube for debug drawing
}

/*
Destructor. Deletes the default shader, and the OpenGL rendering context.
*/
OGLRenderer::~OGLRenderer(void)	{
	SDL_GL_DeleteContext(glContext);
}

/*
Returns TRUE if everything in the constructor has gone to plan.
Check this to end the application if necessary...
*/
bool OGLRenderer::HasInitialised() const{
	return init;
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
Swaps the buffers, ready for the next frame's rendering. Should be called
every frame, at the end of RenderScene(), or whereever appropriate for
your application.
*/
void OGLRenderer::SwapBuffers() {
	//We call the windows OS SwapBuffers on win32. Wrapping it in this 
	//function keeps all the tutorial code 100% cross-platform (kinda).
	::SwapBuffers(deviceContext);
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

void OGLRenderer::setTextureRepeating(GLuint texture, bool repeating) {
	auto value = repeating ? GL_REPEAT : GL_CLAMP;
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, value);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, value);
	glBindTexture(GL_TEXTURE_2D, 0);
}

#ifdef OPENGL_DEBUGGING
void OGLRenderer::DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)	{
		string sourceName;
		string typeName;
		string severityName;

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

		std::cout << "OpenGL Debug Output: " + sourceName + ", " + typeName + ", " + severityName + ", " + string(message) << "\n";
}
#endif




