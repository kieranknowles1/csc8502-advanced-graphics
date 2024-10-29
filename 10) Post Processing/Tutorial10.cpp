#include "../nclgl/window.h"
#include "Renderer.h"

int main() {	
	Window w("Post Processing!", 1280, 720,false);
	w.SetTitle("Adios, WinAPI");
	
	Renderer renderer(w);
	if(!renderer.HasInitialised()) {
		return -1;
	}

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