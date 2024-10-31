#include "../nclgl/Window.h"
#include "Renderer.h"


int main() {
	Window w("Deferred Rendering!", 1280,720,false); //This is all boring win32 window creation stuff!
	Renderer renderer(w); //This handles all the boring OGL 3.2 initialisation stuff, and sets up our tutorial!

	w.LockMouseToWindow(true);
	w.ShowOSPointer(false);

	while(w.UpdateWindow() && !w.GetKeyboard()->KeyDown(SDL_SCANCODE_ESCAPE)){
		renderer.UpdateScene(w.GetTimer()->GetTimeDeltaSeconds());
		renderer.RenderScene();
		w.swapBuffers();
		if (w.GetKeyboard()->KeyDown(SDL_SCANCODE_F5)) {
			Shader::ReloadAllShaders();
		}
	}

	return 0;
}