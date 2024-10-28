#include <nclgl/Window.h>
#include "Renderer.h"

int main()	{
	Window w("My First OpenGL Triangle!", 1280, 720, false);

	if(!w.HasInitialised()) {
		std::cout << "Window failed to initialise!" << std::endl;
		return -1;
	}

	Renderer renderer(w);
	if(!renderer.HasInitialised()) {
		std::cout << "Renderer failed to initialise!" << std::endl;
		return -1;
	}

	while(w.UpdateWindow()  && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)){
		renderer.RenderScene();
		renderer.SwapBuffers();
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_F5)) {
			Shader::ReloadAllShaders();
		}
	}
	return 0;
}
